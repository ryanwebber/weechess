#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/move.h>

TEST_CASE("Move binary packing", "")
{
    using namespace weechess;

    auto move = Move::by_moving(Piece(Piece::Type::Knight, Color::White), Location::H8, Location::A3);
    CHECK(move.start_location() == Location::H8);
    CHECK(move.end_location() == Location::A3);
    CHECK(move.moving_piece() == Piece(Piece::Type::Knight, Color::White));
    CHECK(move.resulting_piece() == Piece(Piece::Type::Knight, Color::White));
}
