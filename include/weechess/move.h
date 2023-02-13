#pragma once

#include <optional>

#include <weechess/location.h>
#include <weechess/piece.h>

namespace weechess {

struct Move {
    Location origin;
    Location destination;
    std::optional<Piece::Piece::Type> promotion {};

    Move(Location origin, Location destination)
        : origin(origin)
        , destination(destination) {};

    Move chromatic_inverse() const { return Move(origin.chromatic_inverse(), destination.chromatic_inverse()); }

    friend bool operator==(Move const&, Move const&) = default;
};

}
