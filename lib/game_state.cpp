#include "fen.h"
#include "game_state.h"

namespace weechess {

    std::vector<Move> generate_legal_moves(const GameState&);

    bool CastleRights::has_rights() const {
        return can_castle_kingside || can_castle_queensize;
    }

    CastleRights CastleRights::none() {
        return CastleRights { false, false };
    }

    CastleRights CastleRights::all() {
        return CastleRights { true, true };
    }

    GameState::GameState()
        : m_turn_to_move(Color::White)
        , m_castle_rights(CastleRights::all())
        , m_en_passant_target(std::nullopt) {}

    GameState::GameState(
        Board board,
        Color turn_to_move,
        PlayerState<CastleRights> castle_rights,
        std::optional<Location> en_passant_target
    )
        : m_board(board)
        , m_turn_to_move(turn_to_move)
        , m_castle_rights(castle_rights)
        , m_en_passant_target(en_passant_target) {}

    const Color& GameState::turn_to_move() const {
        return m_turn_to_move;
    }

    const PlayerState<CastleRights>& GameState::castle_rights() const {
        return m_castle_rights;
    }

    const std::optional<Location>& GameState::en_passant_target() const {
        return m_en_passant_target;
    }

    std::string GameState::to_fen() const {
        return fen::to_fen(*this);
    }

    std::optional<GameState> GameState::from_fen(std::string_view fen_sv) {
        return fen::from_fen(fen_sv);
    }

    bool GameState::is_legal_move(const Move &move) const {
        auto legal_moves = this->legal_moves();
        // return std::find(legal_moves.begin(), legal_moves.end(), move) != legal_moves.end();
        return false;
    }

    std::span<const Move> GameState::legal_moves() const {
        return const_cast<LazyLegalMoves&>(m_legal_moves).get_or_compute(*this);
    }

    bool GameState::is_check() const {
        return false;
    }

    bool GameState::is_checkmate() const {
        return is_check() && legal_moves().empty();
    }

    bool GameState::is_stalemate() const {
        return !is_check() && legal_moves().empty();
    }

    GameState GameState::new_game() {
        return GameState::from_fen(fen::initial_gamestate_fen).value();
    }

    const Board& GameState::board() const {
        return m_board;
    }
    
    std::span<const Move> GameState::LazyLegalMoves::get_or_compute(const GameState& game_state) {
        if (!m_legal_moves.has_value()) {
            m_legal_moves = generate_legal_moves(game_state);
        }

        return m_legal_moves.value();
    }

    std::vector<Move> generate_legal_moves(const GameState&) {
        // TODO: Implement this
        return {
            Move { Location::from_rank_and_file(1, 3), Location::from_rank_and_file(2, 3) },
            Move { Location::from_rank_and_file(1, 3), Location::from_rank_and_file(3, 3) },
        };
    }
}
