#include "fen.h"
#include "log.h"
#include <weechess/game_state.h>

namespace weechess {

GameState::Analysis analyze(const GameState& game_state);

bool CastleRights::has_rights() const { return can_castle_kingside || can_castle_queensize; }

CastleRights CastleRights::none() { return CastleRights { false, false }; }

CastleRights CastleRights::all() { return CastleRights { true, true }; }

GameState::GameState()
    : m_turn_to_move(Color::White)
    , m_castle_rights(CastleRights::all())
    , m_en_passant_target(std::nullopt)
{
}

GameState::GameState(
    Board board, Color turn_to_move, PlayerState<CastleRights> castle_rights, std::optional<Location> en_passant_target)
    : m_board(board)
    , m_turn_to_move(turn_to_move)
    , m_castle_rights(castle_rights)
    , m_en_passant_target(en_passant_target)
{
}

const Color& GameState::turn_to_move() const { return m_turn_to_move; }

const PlayerState<CastleRights>& GameState::castle_rights() const { return m_castle_rights; }

const std::optional<Location>& GameState::en_passant_target() const { return m_en_passant_target; }

std::string GameState::to_fen() const { return fen::to_fen(*this); }

std::optional<GameState> GameState::from_fen(std::string_view fen_sv) { return fen::from_fen(fen_sv); }

GameState GameState::new_game() { return GameState::from_fen(fen::initial_gamestate_fen).value(); }

const Board& GameState::board() const { return m_board; }

const GameState::Analysis& GameState::analysis() const
{
    if (!m_analysis) {
        auto mut_this = const_cast<GameState*>(this);
        mut_this->m_analysis = analyze(*this);
    }

    return *m_analysis;
}

GameState GameState::as_monochromatic(Color perspective) const
{
    if (perspective == m_turn_to_move) {
        return *this;
    }

    auto castle_rights = m_castle_rights.flipped();

    auto en_passant_target = m_en_passant_target;
    if (en_passant_target) {
        en_passant_target = en_passant_target->chromatic_inverse();
    }

    Board board;
    for (auto i = 0; i < Board::cell_count; i++) {
        Location l(i);
        board.set_piece_at(l.chromatic_inverse(), m_board.piece_at(l).chromatic_inverse());
    }

    return GameState { board, perspective, castle_rights, en_passant_target };
}

std::optional<GameState> GameState::by_performing_move(const GameState& game_state, const Move& move, MoveDetail*)
{
    if (!game_state.board().piece_at(move.origin).is(game_state.turn_to_move())) {
        return {};
    }

    if (!game_state.board().piece_at(move.origin).exists()) {
        return {};
    }

    if (!game_state.analysis().is_legal_move(move)) {
        return {};
    }

    // TODO: This does not account for:
    //  * Castle rights
    //  * Castling
    //  * En passant
    //  * Promotion

    Board board(game_state.board());
    board.set_piece_at(move.destination, board.piece_at(move.origin));
    board.set_piece_at(move.origin, Piece::none());

    return GameState { board, invert_color(game_state.turn_to_move()), game_state.castle_rights(), {} };
}

GameState::Analysis::Analysis(
    bool is_check, std::vector<Move> legal_moves, std::array<uint8_t, Board::cell_count> threat_map)
    : m_is_check(is_check)
    , m_legal_moves(legal_moves)
    , m_threat_map(threat_map)
    , m_legal_moves_by_location({})
{
    auto legal_moves_begin = m_legal_moves.begin();
    while (legal_moves_begin != m_legal_moves.end()) {
        auto legal_moves_end = std::find_if(legal_moves_begin, m_legal_moves.end(), [&](const Move& move) {
            return move.origin != legal_moves_begin->origin;
        });

        auto legal_moves_span = std::span<const Move> { &*legal_moves_begin,
            static_cast<size_t>(std::distance(legal_moves_begin, legal_moves_end)) };
        m_legal_moves_by_location[legal_moves_begin->origin.offset] = legal_moves_span;
        legal_moves_begin = legal_moves_end;
    }
}

bool GameState::Analysis::is_check() const { return m_is_check; }
bool GameState::Analysis::is_checkmate() const { return legal_moves().empty() && is_check(); }
bool GameState::Analysis::is_stalemate() const { return legal_moves().empty() && !is_check(); }

std::span<const Move> GameState::Analysis::legal_moves() const { return m_legal_moves; }
std::span<const Move> GameState::Analysis::legal_moves_from(Location location) const
{
    return m_legal_moves_by_location[location.offset];
}

bool GameState::Analysis::is_legal_move(const Move move) const
{
    return std::find(m_legal_moves.begin(), m_legal_moves.end(), move) != m_legal_moves.end();
}

std::span<const uint8_t> GameState::Analysis::threat_map() const { return m_threat_map; }

namespace {
    using GeneratorFn = std::function<void(std::vector<Move>&, Location, const GameState&)>;
    extern const std::array<GeneratorFn, 7> pseudo_legal_moves_for_piece;
}

GameState::Analysis analyze(const GameState& game_state)
{
    auto mono_game_state = game_state.as_monochromatic(Color::White);

    bool is_check = false;
    std::vector<Move> pseudo_legal_moves;
    std::array<uint8_t, Board::cell_count> threat_map {};

    for (auto i = 0; i < Board::cell_count; i++) {
        Location location(i);
        Piece piece = mono_game_state.board().piece_at(location);
        if (piece.exists() && piece.is(Color::White))
            pseudo_legal_moves_for_piece[static_cast<uint8_t>(piece.type())](
                pseudo_legal_moves, location, mono_game_state);
    }

    if (game_state.turn_to_move() != mono_game_state.turn_to_move()) {
        for (auto& move : pseudo_legal_moves) {
            move = move.chromatic_inverse();
        }
    }

    return GameState::Analysis { is_check, pseudo_legal_moves, threat_map };
}

namespace {
    const std::array<GeneratorFn, 7> pseudo_legal_moves_for_piece = {
        [](std::vector<Move>& moves, Location location, const GameState&) {
            // Piece::Type::None
        },
        [](std::vector<Move>& moves, Location location, const GameState& game_state) {
            // Piece::Type::Pawn

            auto one_hop = location.offset_by(Location::Down);
            auto two_hop = location.offset_by(Location::Down * 2);

            if (one_hop && game_state.board().piece_at(*one_hop).is_none()) {
                moves.emplace_back(location, *one_hop);

                if (two_hop && game_state.board().piece_at(*two_hop).is_none() && location.rank() == 1) {
                    moves.emplace_back(location, *two_hop);
                }
            }

            std::array<std::optional<Location>, 2> capture_locations = {
                location.offset_by(Location::Down, Location::Left),
                location.offset_by(Location::Down, Location::Right),
            };

            for (auto capture_location : capture_locations) {
                if (capture_location && game_state.board().piece_at(*capture_location).is(Color::Black)) {
                    moves.emplace_back(location, *capture_location);
                }
            }
        },
        [](std::vector<Move>& moves, Location location, const GameState&) {
            // Piece::Type::Knight
        },
        [](std::vector<Move>& moves, Location location, const GameState&) {
            // Piece::Type::Bishop
        },
        [](std::vector<Move>& moves, Location location, const GameState&) {
            // Piece::Type::Rook
        },
        [](std::vector<Move>& moves, Location location, const GameState&) {
            // Piece::Type::Queen
        },
        [](std::vector<Move>& moves, Location location, const GameState&) {
            // Piece::Type::King
        },
    };
}

} // namespace weechess
