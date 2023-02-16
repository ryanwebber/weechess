#include <catch2/catch_test_macros.hpp>

#include <weechess/game_state.h>

TEST_CASE("Basic fen string parsing", "[fen]")
{
    using namespace weechess;
    auto gs = GameState::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQq - 0 1");
    REQUIRE(gs.has_value());
    REQUIRE(gs->board().piece_at(Location::A1) == Piece(Piece::Type::Rook, Color::White));
    REQUIRE(gs->board().piece_at(Location::D8) == Piece(Piece::Type::Queen, Color::Black));
    REQUIRE(gs->castle_rights()[Color::White].can_castle_kingside == true);
    REQUIRE(gs->castle_rights()[Color::Black].can_castle_kingside == false);
    REQUIRE(gs->turn_to_move() == Color::Black);
}
