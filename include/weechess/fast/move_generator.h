#pragma once

#include <array>
#include <optional>

#include <weechess/fast/bit_board.h>
#include <weechess/game_state.h>
#include <weechess/move.h>

namespace weechess::fast {

namespace internal {
    BitBoard rook_attacks(Location, BitBoard blockers);
    BitBoard bishop_attacks(Location, BitBoard blockers);
}

class MoveGenerator {
public:
    struct Request {
        Color turn_to_move;
        ColorMap<CastleRights> castle_rights;
        std::optional<Location> en_passant_target;
        std::array<BitBoard, Piece::type_mask | Piece::color_mask> occupancy;

        Request();

        void set_turn_to_move(Color color);
        void set_en_passant_target(const Location& location);
        void set_castle_rights(Color color, CastleRights rights);
        void add_piece(const Piece&, const Location&);
    };

    struct Result { };

    MoveGenerator() = default;

    Result execute(const Request&) const;
};

}
