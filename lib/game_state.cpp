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
std::span<const Move> MoveSet::legal_moves_from(Location) const
{
    assert(false);
    return {};
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
    , m_move_set()
{
}

GameState::GameState(
    Board board, Color turn_to_move, ColorMap<CastleRights> castle_rights, std::optional<Location> en_passant_target)
    : m_board(std::move(board))
    , m_turn_to_move(turn_to_move)
    , m_castle_rights(castle_rights)
    , m_en_passant_target(en_passant_target)
    , m_move_set()
{
}

const Color& GameState::turn_to_move() const { return m_turn_to_move; }
const ColorMap<CastleRights>& GameState::castle_rights() const { return m_castle_rights; }
const std::optional<Location>& GameState::en_passant_target() const { return m_en_passant_target; }

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

std::string GameState::to_fen() const { return fen::to_fen(*this); }
std::optional<GameState> GameState::from_fen(std::string_view fen_sv) { return fen::from_fen(fen_sv); }

GameState GameState::new_game() { return GameState::from_fen(fen::initial_gamestate_fen).value(); }

std::optional<GameState> GameState::by_performing_move(const GameState& game_state, const Move& move)
{
    // Check that the piece at the start location is the same color as the turn to move
    if (game_state.board().piece_at(move.start_location()) != move.moving_piece()) {
        log::debug("Move {} does not seem to fit the board right (expected: {})",
            move.to_short_algebraic_notation(),
            game_state.board().piece_at(move.start_location()).to_letter());
        return {};
    }

    // Make sure this move is actually legal
    if (!game_state.move_set().is_legal_move(move)) {
        log::debug("Move {} is not a listed legal move", move.to_short_algebraic_notation());
        return {};
    }

    // TODO: This does not account for:
    //  * Castle rights
    //  * Castling
    //  * En passant
    //  * Promotion

    auto moving_piece = move.moving_piece();
    auto resulting_piece = move.resulting_piece();

    Board::Buffer buffer = game_state.board().piece_buffer();

    // Start and end positions of the piece
    buffer.occupancy_for(moving_piece).unset(move.start_location());
    buffer.occupancy_for(resulting_piece).set(move.end_location());

    // TODO: Handle captures by removing stuff off the other color board

    Board board(std::move(buffer));

    return GameState(std::move(board), invert_color(game_state.turn_to_move()), game_state.castle_rights(), {});
}

} // namespace weechess
