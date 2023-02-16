#include <catch2/catch_test_macros.hpp>

#include <weechess/bit_board.h>

TEST_CASE("Least significant bit math", "[bitboard]")
{
    using namespace weechess;

    BitBoard bb1(0b0001);
    CHECK(bb1.lsb().offset == 0);

    BitBoard bb3(0b1100);
    CHECK(bb3.lsb().offset == 2);

    BitBoard bb8(0b1101011100000000);
    CHECK(bb8.lsb().offset == 8);
}
