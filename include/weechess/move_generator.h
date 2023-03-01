#pragma once

#include <array>
#include <optional>
#include <vector>

#include <weechess/board.h>
#include <weechess/game_state.h>
#include <weechess/move.h>

namespace weechess {

class MoveGenerator {
public:
    struct Result {
        std::vector<LegalMove> legal_moves;
    };

    MoveGenerator() = default;

    Result execute(const GameSnapshot&) const;
};

}
