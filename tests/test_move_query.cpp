#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <weechess/board.h>
#include <weechess/move_query.h>

TEST_CASE("PGN move parsing")
{
    using namespace weechess;

    SECTION("Valid strings")
    {
        std::vector<std::string> move_strings = {
            "e5",
            "Bxe5",
            "exd5",
            "Nf3",
            "O-O-O",
            "O-O",
            "Qh4e1",
            "Qh4xe1",
            "R1a3",
            "Rdf8",
            "e8=Q",
            "Qxa7+",
            "Rhxh6#",
            "d1=Q+",
        };

        for (const auto& move_string : move_strings) {
            auto query = PGNMoveQuery::from(move_string);
            INFO("Parsing move string: " << move_string);
            CHECK(query.has_value());
        }
    }

    SECTION("Invalid strings")
    {
        std::vector<std::string> move_strings = {
            "",
            "abcde3",
        };

        for (const auto& move_string : move_strings) {
            auto query = PGNMoveQuery::from(move_string);
            INFO("Parsing move string: " << move_string);
            CHECK(!query.has_value());
        }
    }

    SECTION("Capture")
    {
        auto query = PGNMoveQuery::from("Bxe5").value();
        CHECK(query.is_capture());
    }

    SECTION("Pawn move")
    {
        auto query = PGNMoveQuery::from("e5").value();
        CHECK(query.piece_type() == Piece::Type::Pawn);
    }

    SECTION("Promotion")
    {
        auto query = PGNMoveQuery::from("e8=Q").value();
        CHECK(query.promotion_type() == Piece::Type::Queen);
        CHECK(query.destination() == Location::E8);
    }

    SECTION("Rank disambiguation")
    {
        auto query = PGNMoveQuery::from("R1a3").value();
        CHECK(query.origin_rank() == Rank(1));
    }

    SECTION("File disambiguation")
    {
        auto query = PGNMoveQuery::from("Rdf8").value();
        CHECK(query.origin_file() == File('D'));
        CHECK(query.piece_type() == Piece::Type::Rook);
    }

    SECTION("Normal move")
    {
        auto query = PGNMoveQuery::from("Nf3").value();
        CHECK(query.piece_type() == Piece::Type::Knight);
        CHECK(query.destination() == Location::F3);
        CHECK(query.is_capture() == false);
        CHECK(query.is_castle(CastleSide::Kingside) == false);
        CHECK(query.is_castle(CastleSide::Queenside) == false);
        CHECK(!query.promotion_type().has_value());
        CHECK(!query.origin_file().has_value());
        CHECK(!query.origin_rank().has_value());
    }

    SECTION("Pawn capture move white")
    {
        auto query = PGNMoveQuery::from("dxc4").value();
        CHECK(query.piece_type() == Piece::Type::Pawn);
        CHECK(query.destination() == Location::C4);
        CHECK(query.origin_file() == File('D'));
        CHECK(query.is_capture() == true);
        CHECK(query.is_castle(CastleSide::Kingside) == false);
        CHECK(query.is_castle(CastleSide::Queenside) == false);
        CHECK(!query.promotion_type().has_value());
        CHECK(!query.origin_rank().has_value());

        Move actual_move(Move::Data(92721UL));

        CHECK(query.test(actual_move));
    }

    SECTION("Pawn capture move black")
    {
        auto query = PGNMoveQuery::from("cxd5").value();
        CHECK(query.piece_type() == Piece::Type::Pawn);
        CHECK(query.destination() == Location::D5);
        CHECK(query.origin_file() == File('C'));
        CHECK(query.is_capture() == true);
        CHECK(query.is_castle(CastleSide::Kingside) == false);
        CHECK(query.is_castle(CastleSide::Queenside) == false);
        CHECK(!query.promotion_type().has_value());
        CHECK(!query.origin_rank().has_value());

        Move actual_move(Move::Data(268537249UL));

        CHECK(query.test(actual_move));
    }
}
