#pragma once

#include <array>
#include <optional>
#include <vector>

#include <weechess/board.h>
#include <weechess/game_state.h>
#include <weechess/move.h>

namespace weechess {

namespace testapi {
    BitBoard rook_attacks(Location, BitBoard blockers);
    BitBoard bishop_attacks(Location, BitBoard blockers);
}

class MoveGenerator {
public:
    class Request {
    private:
        Board m_board;
        Color m_turn_to_move;
        ColorMap<CastleRights> m_castle_rights;
        std::optional<Location> m_en_passant_target;

    public:
        Request(Board);

        void set_turn_to_move(Color color);
        void set_en_passant_target(const Location& location);
        void set_castle_rights(Color color, CastleRights rights);

        const Board& board() const { return m_board; }
        const Color& turn_to_move() const { return m_turn_to_move; }
        const ColorMap<CastleRights>& castle_rights() const { return m_castle_rights; }
        const std::optional<Location>& en_passant_target() const { return m_en_passant_target; }
    };

    struct Result {
        std::vector<Move> legal_moves;
    };

    MoveGenerator() = default;

    Result execute(const Request&) const;
};

}
