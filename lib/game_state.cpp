#include "fen.h"
#include "log.h"
#include <weechess/game_state.h>

namespace weechess {

bool CastleRights::has_rights() const { return can_castle_kingside || can_castle_queenside; }

std::span<const Move> MoveSet::legal_moves() const { return {}; };
std::span<const Move> MoveSet::legal_moves_from(Location) const { return {}; }
bool MoveSet::is_legal_move(const Move&) const { return false; }

GameState::GameState()
    : m_turn_to_move(Color::White)
    , m_castle_rights(CastleRights::all())
    , m_en_passant_target({})
{
}

GameState::GameState(
    Board board, Color turn_to_move, ColorMap<CastleRights> castle_rights, std::optional<Location> en_passant_target)
    : m_board(std::move(board))
    , m_turn_to_move(turn_to_move)
    , m_castle_rights(castle_rights)
    , m_en_passant_target(en_passant_target)
{
}

const Color& GameState::turn_to_move() const { return m_turn_to_move; }
const ColorMap<CastleRights>& GameState::castle_rights() const { return m_castle_rights; }
const std::optional<Location>& GameState::en_passant_target() const { return m_en_passant_target; }

bool GameState::is_check() const { return false; }
bool GameState::is_checkmate() const { return move_set().legal_moves().empty() && is_check(); }
bool GameState::is_stalemate() const { return move_set().legal_moves().empty() && !is_check(); }

const Board& GameState::board() const { return m_board; }
const MoveSet GameState::move_set() const { return m_move_set; }

std::string GameState::to_fen() const { return fen::to_fen(*this); }
std::optional<GameState> GameState::from_fen(std::string_view fen_sv) { return fen::from_fen(fen_sv); }

GameState GameState::new_game() { return GameState::from_fen(fen::initial_gamestate_fen).value(); }

std::optional<GameState> GameState::by_performing_move(const GameState& game_state, const Move& move)
{
    // TODO: check that the move is made by the right color piece

    if (!game_state.move_set().is_legal_move(move)) {
        return {};
    }

    // TODO: This does not account for:
    //  * Castle rights
    //  * Castling
    //  * En passant
    //  * Promotion

    Board board(game_state.board());
    // TODO: Apply the piece to the board

    return GameState { std::move(board), invert_color(game_state.turn_to_move()), game_state.castle_rights(), {} };
}

} // namespace weechess
