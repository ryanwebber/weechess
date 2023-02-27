#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/game_state.h>
#include <weechess/move_generator.h>
#include <weechess/move_query.h>

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

    SECTION("Pawn capture")
    {
        auto gs = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/5n2/PPP1N1PP/RNBQK2R w KQ - 1 8").value();

        auto moves = gs.move_set().find(LocationMoveQuery(Location::G2, Location::F3));
        REQUIRE(moves.size() == 1);

        auto gs2 = GameState::by_performing_move(gs, moves[0]);
        REQUIRE(gs2.has_value());

        CHECK(gs2->to_fen() == "rnbq1k1r/pp1Pbppp/2p5/8/2B5/5P2/PPP1N2P/RNBQK2R b KQ - 0 8");
    }

    SECTION("Castling moves")
    {
        auto gs = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8").value();

        auto moves = gs.move_set().find(CastleMoveQuery(CastleSide::Kingside));
        REQUIRE(moves.size() == 1);

        auto gs2 = GameState::by_performing_move(gs, moves[0]);
        REQUIRE(gs2.has_value());

        CHECK(gs2->to_fen() == "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQ1RK1 b - - 2 8");
    }
}
