#pragma once

#include <weechess/game_state.h>
#include <weechess/piece.h>

namespace weechess {

struct Evaluation {
    int relative_score;

    bool is_better_than(const Evaluation& other, const Color& perspective) const;
};

class Evaluator {
public:
    Evaluator() = default;
    void doThing() {};

    Evaluation evaluate(const GameState&) const;

    static const Evaluator default_instance;
};

}
