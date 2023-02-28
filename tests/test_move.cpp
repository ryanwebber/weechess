#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/game_state.h>
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

TEST_CASE("Move short algebraic notation")
{
    using namespace weechess;

    auto game_state = GameState::from_fen("8/8/1prp4/1P1P4/8/8/8/8 w - - 0 1").value();
    auto moves = game_state.move_set().legal_moves();
    REQUIRE(moves.size() == 2);

    auto left_capture = std::find_if(
        moves.begin(), moves.end(), [](const auto& move) { return move.start_location() == Location::D5; });

    auto right_capture = std::find_if(
        moves.begin(), moves.end(), [](const auto& move) { return move.start_location() == Location::B5; });

    REQUIRE(left_capture != moves.end());
    REQUIRE(right_capture != moves.end());

    CHECK(left_capture->san_notation(game_state) == "dxc6");
    CHECK(right_capture->san_notation(game_state) == "bxc6");
}
