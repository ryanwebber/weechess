#pragma once

#include <optional>

#include "location.h"
#include "piece.h"

namespace weechess {

    struct Move {
        Location origin;
        Location destination;
        std::optional<Piece::Piece::Type> promotion;

        friend bool operator==(Move const&, Move const&) = default;
    };

}
