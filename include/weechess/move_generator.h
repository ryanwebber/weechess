#pragma once

#include <array>
#include <optional>

#include <weechess/bit_board.h>
#include <weechess/game_state.h>
#include <weechess/move.h>

namespace weechess {

class MoveGenerator {
public:
    struct Request {
        Color turn_to_move;
        ColorMap<CastleRights> castle_rights;
        std::optional<Location> en_passant_target;
        std::array<ColorMap<BitBoard>, 7> occupancy;

        Request(Color turn_to_move, ColorMap<CastleRights> castle_rights);

        void add_piece(const Piece&, const Location&);
        void set_en_passant_target(const Location& location);
    };

    struct Result { };

    MoveGenerator() = default;

    Result execute(const Request&) const;
};

}
