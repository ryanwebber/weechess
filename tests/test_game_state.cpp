#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/bit_board.h>
#include <weechess/game_state.h>
#include <weechess/move_generator.h>
#include <weechess/move_query.h>

struct TestCase {
    std::string begin_fen;
    std::vector<std::shared_ptr<weechess::MoveQuery>> moves;
    std::string end_fen;

    void run()
    {
        auto gs = weechess::GameState::from_fen(begin_fen);
        auto moves_to_make = moves.begin();
        auto i = 0;
        while (gs.has_value() && moves_to_make != moves.end()) {
            REQUIRE(gs.has_value());

            INFO("[" << (i++) << "] FEN: \"" << gs->to_fen() << "\"");
            auto moves = gs->move_set().find(**moves_to_make);
            REQUIRE(moves.size() == 1);

            UNSCOPED_INFO("Applying move:\n" << gs->verbose_description(moves[0]));

            gs = weechess::GameState::by_performing_move(*gs, moves[0]);
            moves_to_make++;
        }

        REQUIRE(gs.has_value());
        CHECK(gs->to_fen() == end_fen);
    }
};

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
        TestCase test_case = {
            .begin_fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/5n2/PPP1N1PP/RNBQK2R w KQ - 1 8",
            .moves = {
                std::make_shared<LocationMoveQuery>(Location::G2, Location::F3),
            },
            .end_fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/5P2/PPP1N2P/RNBQK2R b KQ - 0 8",
        };

        test_case.run();
    }

    SECTION("Pawn double pushes")
    {
        TestCase test_case = {
            .begin_fen = "8/1p6/8/8/8/8/1P6/8 w - - 1 8",
            .moves = {
                std::make_shared<LocationMoveQuery>(Location::B2, Location::B4),
                std::make_shared<LocationMoveQuery>(Location::B7, Location::B5),
            },
            .end_fen = "8/8/8/1p6/1P6/8/8/8 w - b6 0 9",
        };

        test_case.run();
    }

    SECTION("En passant")
    {
        TestCase test_case = {
            .begin_fen = "8/2p5/8/1P6/8/8/8/8 b - - 1 8",
            .moves = {
                std::make_shared<LocationMoveQuery>(Location::C7, Location::C5),
                std::make_shared<LocationMoveQuery>(Location::B5, Location::C6),
            },
            .end_fen = "8/8/2P5/8/8/8/8/8 b - - 0 9",
        };

        test_case.run();
    }

    SECTION("Castling white queenside")
    {
        TestCase test_case = {
            .begin_fen = "8/8/8/5q2/8/8/8/R3K2R w KQ - 1 8",
            .moves = {
                std::make_shared<CastleMoveQuery>(CastleSide::Queenside),
            },
            .end_fen = "8/8/8/5q2/8/8/8/2KR3R b - - 2 8",
        };

        test_case.run();
    }

    SECTION("Castling white kingside")
    {
        TestCase test_case = {
            .begin_fen = "8/8/8/8/8/8/8/R3K2R w KQ - 1 8",
            .moves = {
                std::make_shared<CastleMoveQuery>(CastleSide::Kingside),
            },
            .end_fen = "8/8/8/8/8/8/8/R4RK1 b - - 2 8",
        };

        test_case.run();
    }

    SECTION("Castling black queenside")
    {
        TestCase test_case = {
            .begin_fen = "r3k2r/8/8/8/8/8/8/8 b kq - 1 8",
            .moves = {
                std::make_shared<CastleMoveQuery>(CastleSide::Queenside),
            },
            .end_fen = "2kr3r/8/8/8/8/8/8/8 w - - 2 9",
        };

        test_case.run();
    }

    SECTION("Castling black kingside")
    {
        TestCase test_case = {
            .begin_fen = "r3k2r/8/8/8/8/8/8/8 b kq - 1 8",
            .moves = {
                std::make_shared<CastleMoveQuery>(CastleSide::Kingside),
            },
            .end_fen = "r4rk1/8/8/8/8/8/8/8 w - - 2 9",
        };

        test_case.run();
    }

    SECTION("Castling in check")
    {
        auto game_state = GameState::from_fen("r3k2r/8/8/8/8/4R3/8/8 b kq - 1 8").value();
        auto kingside = game_state.move_set().find(CastleMoveQuery(CastleSide::Kingside));
        auto queenside = game_state.move_set().find(CastleMoveQuery(CastleSide::Queenside));

        CHECK(kingside.size() == 0);
        CHECK(queenside.size() == 0);
    }

    SECTION("Castling through piece")
    {
        auto game_state = GameState::from_fen("r3kN1r/8/8/8/8/8/8/8 b kq - 1 8").value();
        auto kingside = game_state.move_set().find(CastleMoveQuery(CastleSide::Kingside));
        auto queenside = game_state.move_set().find(CastleMoveQuery(CastleSide::Queenside));

        CHECK(kingside.size() == 0);
        CHECK(queenside.size() == 1);
    }
}
