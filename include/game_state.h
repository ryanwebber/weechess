#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "board.h"
#include "piece.h"
#include "player_state.h"

namespace weechess {

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

        std::string to_fen() const;

        static std::optional<GameState> from_fen(std::string_view);
        static GameState new_game();
    private:
        Board m_board;
        Color m_turn_to_move;

        PlayerState<CastleRights> m_castle_rights;
        std::optional<Location> m_en_passant_target;
    };

}
