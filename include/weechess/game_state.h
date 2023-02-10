#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <weechess/board.h>
#include <weechess/move.h>
#include <weechess/piece.h>
#include <weechess/player_state.h>

namespace weechess {

    struct MoveDetail {
        std::optional<Piece> capture;
    };

    struct CastleRights {
        bool can_castle_kingside;
        bool can_castle_queensize;

        bool has_rights() const;

        static CastleRights none();
        static CastleRights all();
    };

    class GameState {
    public:
        GameState();
        GameState(
            Board board,
            Color turn_to_move,
            PlayerState<CastleRights> castle_rights,
            std::optional<Location> en_passant_target);

        const Board& board() const;
        const Color& turn_to_move() const;
        const PlayerState<CastleRights>& castle_rights() const;
        const std::optional<Location>& en_passant_target() const;

        bool is_legal_move(const Move&) const;
        std::span<const Move> legal_moves() const;

        bool is_check() const;
        bool is_checkmate() const;
        bool is_stalemate() const;

        std::string to_fen() const;

        static std::optional<GameState> from_fen(std::string_view);
        static GameState new_game();

        static std::optional<GameState> by_performing_move(const GameState&, const Move&, MoveDetail*);
    private:

        Board m_board;
        Color m_turn_to_move;

        PlayerState<CastleRights> m_castle_rights;
        std::optional<Location> m_en_passant_target;
    };
}
