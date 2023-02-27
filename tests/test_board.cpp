#include <array>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/board.h>

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
