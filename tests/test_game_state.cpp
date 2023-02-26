#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/comptime.h>
#include <weechess/game_state.h>
#include <weechess/move_generator.h>

TEST_CASE("Applying moves to game states", "[rules]")
{
    using namespace weechess;

    SECTION("Legally generated moves are accepted")
    {
        auto gs = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8").value();

        const auto& moves = gs.move_set().legal_moves();
        CHECK(moves.size() != 0);

        for (const auto& move : moves) {
            const auto& gs_prime = GameState::by_performing_move(gs, move);
            INFO(gs.verbose_description(move));
            CHECK(gs_prime.has_value());
        }
    }
}
