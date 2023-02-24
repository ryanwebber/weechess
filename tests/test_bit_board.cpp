#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <weechess/fast/bit_board.h>

TEST_CASE("Least significant bit math", "[bitboard]")
{
    using namespace weechess;
    using namespace weechess::fast;

    // Random bits to use as noise in the test data,
    // just to make sure the LSB calculations aren't
    // thrown off by bits in the upper half of the data.
    constexpr std::array<BitBoard, 8> noise = {
        // clang-format off
        BitBoard(0x344e7f44823d5b0dULL),
        BitBoard(0xe85af31ee6822221ULL),
        BitBoard(0x0000000000000000ULL),
        BitBoard(0x893262894290bf34ULL),
        BitBoard(0xbffc28938dee062fULL),
        BitBoard(0xa2f8df60487ed0b0ULL),
        BitBoard(0xd4f6b5bd124db82cULL),
        BitBoard(0xb97e27a6682fc0d7ULL),
        // clang-format on
    };

    for (auto i = 0; i < 63; i++) {
        BitBoard mask(~((1ULL << i) - 1));
        BitBoard lsb = 0;
        lsb.set(i);

        // Construct data with random data in the upper bits, such that
        // the final data has the lsb at the ith position (x is random noise):
        // 0bxxx...xxxxx100000...0
        //               <-- i -->
        BitBoard bb = ((lsb | noise[i % noise.size()]) & mask);
        CHECK(bb.lsb()->offset == i);
    }

    // Some other explicit tests
    BitBoard bb1(0b0001);
    CHECK(bb1.lsb()->offset == 0);

    BitBoard bb3(0b1100);
    CHECK(bb3.lsb()->offset == 2);

    BitBoard bb8(0b1101011100000000);
    CHECK(bb8.lsb()->offset == 8);

    BitBoard bb0;
    CHECK(!bb0.lsb().has_value());
}

TEST_CASE("BitBoard ostream formatting", "[bitboard]")
{
    using namespace weechess;
    using namespace weechess::fast;

    BitBoard bb;
    bb.set(Location::B1);
    bb.set(Location::G1);
    bb.set(Location::A8);

    std::stringstream ss;
    ss << bb << std::endl;

    std::string expected = "1.......\n"
                           "........\n"
                           "........\n"
                           "........\n"
                           "........\n"
                           "........\n"
                           "........\n"
                           ".1....1.\n";

    CHECK(ss.str() == expected);
}
