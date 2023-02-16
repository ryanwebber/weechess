#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/game_state.h>
#include <weechess/move_generator.h>

uint64_t count_moves(weechess::GameState gs, int depth)
{
    if (depth == 0) {
        return 1;
    } else if (depth == 1) {
        return gs.analysis().legal_moves().size();
    }

    uint64_t count = 0;
    for (auto& move : gs.analysis().legal_moves()) {
        auto new_gs = weechess::GameState::by_performing_move(gs, move, nullptr).value();
        count += count_moves(new_gs, depth - 1);
    }

    return count;
}

TEST_CASE("Legal move generation", "[rules][!mayfail]")
{
    using namespace weechess;

    // https://www.chessprogramming.org/Perft_Results#Position_5
    auto gs = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8").value();
    CHECK(count_moves(gs, 1) == 44);
    CHECK(count_moves(gs, 2) == 1486);
    CHECK(count_moves(gs, 3) == 62379);
    CHECK(count_moves(gs, 4) == 2103487);
    CHECK(count_moves(gs, 5) == 89941194);
}
