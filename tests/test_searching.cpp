#include <catch2/catch_test_macros.hpp>

#include <weechess/game_state.h>
#include <weechess/search_executor.h>

TEST_CASE("Searching for obviously good moves", "[search]")
{
    using namespace weechess;
    SECTION("Forced mate in 3")
    {
        auto game_state = GameState::from_fen("r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 1 1").value();
        auto result = SearchExecutor::search(game_state, 5);
        REQUIRE(result.best_line.size() > 0);
        CHECK(result.best_line[0].start_location() == Location::C4);
        CHECK(result.best_line[0].end_location() == Location::B5);
        CHECK(result.evaluation == Evaluation::mate_in(6));
    }
}
