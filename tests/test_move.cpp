#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/move.h>

TEST_CASE("Move binary packing")
{
    using namespace weechess;

    SECTION("Regular move")
    {
        auto move = Move::by_moving(Piece(Piece::Type::Knight, Color::White), Location::H8, Location::A3);
        CHECK(move.start_location() == Location::H8);
        CHECK(move.end_location() == Location::A3);
        CHECK(move.moving_piece() == Piece(Piece::Type::Knight, Color::White));
        CHECK(move.resulting_piece() == Piece(Piece::Type::Knight, Color::White));
    }

    SECTION("Promotion")
    {
        auto piece = Piece(Piece::Type::Pawn, Color::White);
        auto move = Move::by_promoting(piece, Location::H7, Location::H8, Piece::Type::Queen);
        REQUIRE(move.is_promotion());
        CHECK(move.moving_piece() == Piece(Piece::Type::Pawn, Color::White));
        CHECK(move.resulting_piece() == Piece(Piece::Type::Queen, Color::White));
    }
}
