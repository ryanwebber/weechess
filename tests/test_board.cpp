#include <array>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/board.h>
#include <weechess/game_state.h>

TEST_CASE("Board masks")
{
    using namespace weechess;

    SECTION("File masks")
    {
        auto mask = File('B').mask();
        std::array<Location, 8> expected = {
            Location::B1,
            Location::B2,
            Location::B3,
            Location::B4,
            Location::B5,
            Location::B6,
            Location::B7,
            Location::B8,
        };

        CHECK(mask == BitBoard::from(expected));
    }

    SECTION("Rank masks")
    {
        auto mask = Rank(3).mask();
        std::array<Location, 8> expected = {
            Location::A3,
            Location::B3,
            Location::C3,
            Location::D3,
            Location::E3,
            Location::F3,
            Location::G3,
            Location::H3,
        };

        CHECK(mask == BitBoard::from(expected));
    }
}

TEST_CASE("Board attack map")
{
    using namespace weechess;
    auto board = GameState::from_fen("rn1q1k1r/pp1Pbppp/2p5/6N1/2B5/5b2/PPP1NnPP/R1BQK2R w KQ - 1 8")->board();

    std::vector<Location> expected_attacks = {
        Location::B1,
        Location::F1,
        Location::G1,
        Location::D2,
        Location::F2,
        Location::A3,
        Location::B3,
        Location::C3,
        Location::D3,
        Location::E3,
        Location::F3,
        Location::G3,
        Location::H3,
        Location::D4,
        Location::E4,
        Location::F4,
        Location::B5,
        Location::D5,
        Location::A6,
        Location::D6,
        Location::E6,
        Location::F7,
        Location::H7,
        Location::C8,
        Location::E8,
    };

    CHECK(board.attacks(Color::White) == BitBoard::from(expected_attacks));
}

TEST_CASE("Checkmate detection")
{
    auto game_state = weechess::GameState::from_fen("7k/6Q1/8/8/8/8/8/5KRq b - - 0 1").value();
    CHECK(game_state.is_checkmate());
}

TEST_CASE("Stalemate detection")
{
    auto game_state = weechess::GameState::from_fen("7k/Q7/8/8/8/8/8/5KR1 b - - 0 1").value();
    CHECK(game_state.is_stalemate());
}
