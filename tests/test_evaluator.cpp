#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/evaluator.h>
#include <weechess/game_state.h>

TEST_CASE("Game state evaluation")
{
    using namespace weechess;
    auto game_state = GameState::from_fen("2kq3n/7p/7p/7p/8/8/8/2K5 b - - 0 8").value();
    auto evaluation = Evaluator::default_instance.evaluate(game_state);
    CHECK(evaluation.relative_score == -150);
}
