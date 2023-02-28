#include <sstream>

#include "fen.h"
#include "log.h"
#include <weechess/game_state.h>
#include <weechess/move_generator.h>

namespace weechess {

bool CastleRights::has_rights() const { return can_castle_kingside || can_castle_queenside; }

MoveSet::MoveSet(std::vector<Move> legal_moves)
    : m_legal_moves(std::move(legal_moves))
{
}

std::span<const Move> MoveSet::legal_moves() const { return m_legal_moves; };
std::vector<Move> MoveSet::legal_moves_from(Location from) const
{
    LocationMoveQuery query(from);
    return find(query);
}

std::vector<Move> MoveSet::find(const MoveQuery& query) const
{
    std::vector<Move> result;
    for (const auto& move : m_legal_moves) {
        if (query.test(move)) {
            result.push_back(move);
        }
    }

    return result;
}

bool MoveSet::is_legal_move(const Move& move) const
{
    return std::find(m_legal_moves.begin(), m_legal_moves.end(), move) != m_legal_moves.end();
}

MoveSet MoveSet::compute_from(const GameState& gs)
{
    MoveGenerator::Request request(gs.board());
    request.set_turn_to_move(gs.turn_to_move());
    request.set_castle_rights(Color::White, gs.castle_rights()[Color::White]);
    request.set_castle_rights(Color::Black, gs.castle_rights()[Color::Black]);

    if (gs.en_passant_target().has_value()) {
        request.set_en_passant_target(gs.en_passant_target().value());
    }

    MoveGenerator generator;
    auto result = generator.execute(request);

    return MoveSet(std::move(result.legal_moves));
}

GameState::GameState()
    : m_turn_to_move(Color::White)
    , m_castle_rights(CastleRights::all())
    , m_en_passant_target({})
{
}

GameState::GameState(Board board,
    Color turn_to_move,
    ColorMap<CastleRights> castle_rights,
    std::optional<Location> en_passant_target,
    size_t halfmove_clock,
    size_t fullmove_number)
    : m_board(std::move(board))
    , m_turn_to_move(turn_to_move)
    , m_castle_rights(castle_rights)
    , m_en_passant_target(en_passant_target)
    , m_halfmove_clock(halfmove_clock)
    , m_fullmove_number(fullmove_number)
{
}

const Color& GameState::turn_to_move() const { return m_turn_to_move; }
const ColorMap<CastleRights>& GameState::castle_rights() const { return m_castle_rights; }
const std::optional<Location>& GameState::en_passant_target() const { return m_en_passant_target; }

size_t GameState::halfmove_clock() const { return m_halfmove_clock; }
size_t GameState::fullmove_number() const { return m_fullmove_number; }

bool GameState::is_check() const { return false; }
bool GameState::is_checkmate() const { return move_set().legal_moves().empty() && is_check(); }
bool GameState::is_stalemate() const { return move_set().legal_moves().empty() && !is_check(); }

const Board& GameState::board() const { return m_board; }
const MoveSet& GameState::move_set() const
{
    if (!m_move_set.has_value()) {
        auto mut_this = const_cast<GameState*>(this);
        mut_this->m_move_set = MoveSet::compute_from(*this);
    }

    return m_move_set.value();
}

std::string GameState::san_notation(const Move& move) const
{
    auto target = move.end_location();
    auto origin = move.start_location();
    auto piece = move.moving_piece();

    std::stringstream ss;

    if (piece.type() == Piece::Type::Pawn && move.is_capture()) {
        ss << static_cast<char>('a' + origin.file());
    } else {
        ss << static_cast<char>(std::toupper(piece.to_letter()));

        std::vector<Location> alternative_possible_sources;
        for (const auto& move : move_set().legal_moves()) {
            if (move.end_location() == target && move.start_location() != origin && move.moving_piece() == piece) {
                alternative_possible_sources.push_back(move.start_location());
            }
        }

        if (alternative_possible_sources.size() == 1) {
            if (alternative_possible_sources[0].file() != origin.file()) {
                ss << static_cast<char>('a' + origin.file());
            } else {
                ss << (origin.rank() + 1);
            }
        } else if (alternative_possible_sources.size() > 1) {
            ss << origin.to_string();
        }
    }

    if (move.is_capture()) {
        ss << 'x';
    }

    ss << target.to_string();

    return ss.str();
}

std::string GameState::verbose_description(const Move& move) const
{
    std::stringstream ss;

    auto gs2 = GameState::by_performing_move(*this, move);
    auto move_from = move.start_location();
    auto move_to = move.end_location();

    ss << "\n";

    for (auto i = 0; i < 8; i++) {
        auto rank = 7 - i;
        ss << (rank + 1) << "   ";

        for (auto file = 0; file < 8; file++) {
            auto loc = Location::from_rank_and_file(rank, file);

            auto piece = board().piece_at(loc);
            auto letter = piece.exists() ? piece.to_letter() : '.';

            if (loc == move_from) {
                ss << '[' << letter << ']';
            } else {
                ss << ' ' << letter << ' ';
            }
        }

        if (rank == 3) {
            ss << "     →     ";
        } else {
            ss << "           ";
        }

        if (gs2.has_value()) {
            for (auto file = 0; file < 8; file++) {
                auto loc = Location::from_rank_and_file(rank, file);

                auto piece = gs2->board().piece_at(loc);
                auto letter = piece.exists() ? piece.to_letter() : '.';

                if (loc == move_from || loc == move_to) {
                    ss << '[' << letter << ']';
                } else {
                    ss << ' ' << letter << ' ';
                }
            }
        } else if (rank == 3) {
            ss << " .  . Invalid Move .  . ";
        } else {
            ss << " .  .  .  .  .  .  .  . ";
        }

        ss << "\n\n";
    }

    ss << "\n";
    ss << "   A  B  C  D  E  F  G  H              A  B  C  D  E  F  G  H";
    ss << "\n";
    ss << "\n";

    ss << "Flags\n";
    ss << "Move:      " << san_notation(move) << "\n";

    ss << "Capture:   ";
    if (move.is_capture()) {
        ss << Piece(move.captured_piece_type(), invert_color(m_turn_to_move)).to_letter();
    } else {
        ss << "-";
    }
    ss << "\n";

    ss << "Promotion: ";
    if (move.is_promotion()) {
        ss << Piece(move.promoted_piece_type(), m_turn_to_move).to_letter();
    } else {
        ss << "-";
    }
    ss << "\n";

    ss << "Castle:    ";
    if (move.is_castle() && move.castle_side() == CastleSide::Kingside) {
        ss << "kingside";
    } else if (move.is_castle() && move.castle_side() == CastleSide::Queenside) {
        ss << "queenside";
    } else {
        ss << "-";
    }
    ss << "\n";

    return ss.str();
}

std::string GameState::to_fen() const { return fen::to_fen(*this); }
std::optional<GameState> GameState::from_fen(std::string_view fen_sv) { return fen::from_fen(fen_sv); }

GameState GameState::new_game() { return GameState::from_fen(fen::initial_gamestate_fen).value(); }

std::optional<GameState> GameState::by_performing_move(const GameState& game_state, const Move& move)
{
    auto other_color = invert_color(game_state.turn_to_move());

    // Check that the piece at the start location is the same color as the turn to move
    if (game_state.board().piece_at(move.start_location()) != move.moving_piece()) {
        log::debug("Move {} does not seem to fit the board right (expected: {})",
            game_state.san_notation(move),
            game_state.board().piece_at(move.start_location()).to_letter());
        return {};
    }

    // Make sure this move is actually legal
    if (!game_state.move_set().is_legal_move(move)) {
        log::debug("Move {} is not a listed legal move", game_state.san_notation(move));
        return {};
    }

    auto moving_piece = move.moving_piece();
    auto resulting_piece = move.resulting_piece();

    Board::Buffer buffer = game_state.board().piece_buffer();

    // Start and end positions of the piece
    buffer.occupancy_for(moving_piece).unset(move.start_location());
    buffer.occupancy_for(resulting_piece).set(move.end_location());

    if (move.is_en_passant()) {
        if (game_state.en_passant_target().has_value()) {
            auto captured_piece = Piece(Piece::Type::Pawn, other_color);
            buffer.occupancy_for(captured_piece).unset(game_state.en_passant_target().value());
        } else {
            log::debug("Move {} is invalid because there is no en passant target", game_state.san_notation(move));
            return {};
        }
    } else if (move.is_capture()) {
        auto captured_piece = Piece(move.captured_piece_type(), other_color);
        buffer.occupancy_for(captured_piece).unset(move.end_location());
    }

    if (move.is_promotion()) {
        buffer.occupancy_for(moving_piece).unset(move.end_location());
        buffer.occupancy_for(resulting_piece).set(move.end_location());
    }

    if (move.is_castle()) {
        if (move.castle_side() == CastleSide::Kingside) {
            auto rook_start = Location::from_rank_and_file(move.start_location().rank(), 7);
            auto rook_end = Location::from_rank_and_file(move.start_location().rank(), 5);
            auto rook = Piece(Piece::Type::Rook, game_state.turn_to_move());
            buffer.occupancy_for(rook).unset(rook_start);
            buffer.occupancy_for(rook).set(rook_end);
        } else {
            auto rook_start = Location::from_rank_and_file(move.start_location().rank(), 0);
            auto rook_end = Location::from_rank_and_file(move.start_location().rank(), 3);
            auto rook = Piece(Piece::Type::Rook, game_state.turn_to_move());
            buffer.occupancy_for(rook).unset(rook_start);
            buffer.occupancy_for(rook).set(rook_end);
        }
    }

    auto halfmove_clock = game_state.halfmove_clock() + 1;
    if (move.is_capture() || move.moving_piece().type() == Piece::Type::Pawn) {
        halfmove_clock = 0;
    }

    auto fullmove_number = game_state.fullmove_number();
    if (game_state.turn_to_move() == Color::Black) {
        fullmove_number++;
    }

    auto castle_rights = game_state.castle_rights();
    if (move.moving_piece().type() == Piece::Type::King) {
        castle_rights[game_state.turn_to_move()].can_castle_kingside = false;
        castle_rights[game_state.turn_to_move()].can_castle_queenside = false;
    } else if (move.moving_piece() == Piece(Piece::Type::Rook, Color::White) && move.start_location() == Location::A1) {
        castle_rights[Color::White].can_castle_queenside = false;
    } else if (move.moving_piece() == Piece(Piece::Type::Rook, Color::White) && move.start_location() == Location::H1) {
        castle_rights[Color::White].can_castle_kingside = false;
    } else if (move.moving_piece() == Piece(Piece::Type::Rook, Color::Black) && move.start_location() == Location::A8) {
        castle_rights[Color::Black].can_castle_queenside = false;
    } else if (move.moving_piece() == Piece(Piece::Type::Rook, Color::Black) && move.start_location() == Location::H8) {
        castle_rights[Color::Black].can_castle_kingside = false;
    }

    auto en_passant_target = std::optional<Location>();
    if (move.is_double_pawn()) {
        auto rank_offset = game_state.turn_to_move() == Color::White ? 1 : -1;
        en_passant_target = move.start_location().offset_by(Location::RankShift { rank_offset });
    }

    return GameState(
        Board(std::move(buffer)), other_color, castle_rights, en_passant_target, halfmove_clock, fullmove_number);
}

static std::optional<GameState> by_performing_moves(const GameState& gs, std::span<const Move> moves)
{
    auto itr = moves.begin();
    std::optional<GameState> current = gs;

    while (itr != moves.end() && current.has_value()) {
        current = GameState::by_performing_move(current.value(), *itr);
        itr++;
    }

    return current;
}

} // namespace weechess
