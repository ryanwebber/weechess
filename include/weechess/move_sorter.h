#pragma once

#include <weechess/game_state.h>
#include <weechess/move.h>

namespace weechess {

class MoveSorter {
public:
    MoveSorter() = default;

    Evaluation evaluate(const LegalMove& move) const;

    bool compare(const LegalMove& lhs, const LegalMove& rhs) const { return evaluate(lhs) > evaluate(rhs); }
    bool operator()(const LegalMove& lhs, const LegalMove& rhs) const { return compare(lhs, rhs); }

    static const MoveSorter default_instance;
};

}
