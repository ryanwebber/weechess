#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/attack_maps.h>
#include <weechess/game_state.h>
#include <weechess/move_generator.h>

// Simple perft implementation - this should be a command
uint64_t do_perft(const weechess::GameState& game_state, int depth, bool print = false)
{
    if (depth == 0) {
        return 1;
    } else if (depth == 1 && !print) {
        return game_state.move_set().legal_moves().size();
    }

    uint64_t count = 0;
    for (const auto& legal_move : game_state.move_set().legal_moves()) {
        auto new_gs = weechess::GameState(legal_move.snapshot());
        auto result = do_perft(new_gs, depth - 1);
        count += result;

        if (print) {
            UNSCOPED_INFO(game_state.san_notation(legal_move.move()) << ": " << result << "\n");
        }
    }

    return count;
}

TEST_CASE("Perft move generation counts", "[!benchmark][perft]")
{
    using namespace weechess;

    SECTION("Initial Position")
    {
        // https://www.chessprogramming.org/Perft_Results#Initial_Position
        auto gs = GameState::new_game();
        CHECK(do_perft(gs, 6) == 119060324);
    }

    SECTION("Position 5")
    {
        // https://www.chessprogramming.org/Perft_Results#Position_5
        auto gs = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8").value();
        CHECK(do_perft(gs, 5, true) == 89941194);
    }
}

TEST_CASE("Rook move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Attacks with blockers", "[comptime]")
    {
        auto location = Location::C3;
        std::array<Location, 4> other_pieces = {
            Location::A1,
            Location::G3,
            Location::C2,
            Location::C6,
        };

        /*

        8  .  .  .  .  .  .  .  .
        7  .  .  .  .  .  .  .  .
        6  .  .  B  .  .  .  .  .
        5  .  .  .  .  .  .  .  .
        4  .  .  .  .  .  .  .  .
        3  .  .  X  .  .  .  B  .
        2  .  .  B  .  .  .  .  .
        1  B  .  .  .  .  .  .  .

           A  B  C  D  E  F  G  H

        */

        auto blockers = BitBoard::from(other_pieces);
        auto attacks = attack_maps::generate_rook_attacks(location, blockers);

        std::array<Location, 10> expectated = {
            // Up
            Location::C4,
            Location::C5,
            Location::C6,

            // Down
            Location::C2,

            // Right
            Location::D3,
            Location::E3,
            Location::F3,
            Location::G3,

            // Left
            Location::B3,
            Location::A3,
        };

        CHECK(attacks == BitBoard::from(expectated));
    }

    SECTION("Attacking moves")
    {
        auto game_state = GameState::from_fen("8/2p5/8/8/4p3/2R1P3/2P5/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        REQUIRE(moves.size() == 7);
    }
}

TEST_CASE("Bishop move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Slides with blockers", "[comptime]")
    {
        auto location = Location::C3;
        std::array<Location, 3> other_pieces = {
            Location::B4,
            Location::A1,
            Location::F6,
        };

        /*

        8  .  .  .  .  .  .  .  .
        7  .  .  .  .  .  .  .  .
        6  .  .  .  .  .  B  .  .
        5  .  .  .  .  .  .  .  .
        4  .  B  .  .  .  .  .  .
        3  .  .  X  .  .  .  .  .
        2  .  .  .  .  .  .  .  .
        1  B  .  .  .  .  .  .  .

           A  B  C  D  E  F  G  H

        */

        auto blockers = BitBoard::from(other_pieces);
        auto attacks = attack_maps::generate_bishop_attacks(location, blockers);

        std::array<Location, 8> expectated = {
            // Top Right
            Location::D4,
            Location::E5,
            Location::F6,

            // Down Right
            Location::D2,
            Location::E1,

            // Down Left
            Location::B2,
            Location::A1,

            // Top Left
            Location::B4,
        };

        CHECK(attacks == BitBoard::from(expectated));
    }

    SECTION("Attacking moves")
    {
        auto game_state = GameState::from_fen("8/8/4p3/4P3/1p6/1pB5/1P6/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        REQUIRE(moves.size() == 4);
    }
}

TEST_CASE("Queen move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Attacking moves")
    {
        auto game_state = GameState::from_fen("8/2p5/4p3/4P3/1p5p/1pQ4P/1P6/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        REQUIRE(moves.size() == 15);
    }
}

TEST_CASE("Pawn move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Attacking squares", "[comptime]")
    {
        {
            auto attacks = attack_maps::generate_pawn_attacks(Location::B2, Color::White);
            std::array<Location, 2> expected = { Location::A3, Location::C3 };
            CHECK(attacks == BitBoard::from(expected));
        }

        {
            auto attacks = attack_maps::generate_pawn_attacks(Location::D3, Color::Black);
            std::array<Location, 2> expected = { Location::C2, Location::E2 };
            CHECK(attacks == BitBoard::from(expected));
        }

        {
            auto black_attacks = attack_maps::generate_pawn_attacks(Location::C4, Color::Black);
            auto white_attacks = attack_maps::generate_pawn_attacks(Location::C4, Color::White);
            CHECK(black_attacks != white_attacks);
        }
    }

    SECTION("Capturing moves")
    {
        auto game_state = GameState::from_fen("8/8/8/8/8/1nr5/2P5/8 w - - 0 1").value();
        auto legal_moves = game_state.move_set().legal_moves();

        REQUIRE(legal_moves.size() == 1);
        CHECK(legal_moves[0].move().start_location() == Location::C2);
        CHECK(legal_moves[0].move().end_location() == Location::B3);
        CHECK(legal_moves[0].move().is_capture());
        CHECK(legal_moves[0].move().captured_piece_type() == Piece::Type::Knight);
    }

    SECTION("Double moves")
    {
        auto game_state = GameState::from_fen("8/8/8/8/8/8/2P5/8 w - - 0 1").value();
        auto legal_moves = game_state.move_set().find(LocationMoveQuery(Location::C2, Location::C4));
        REQUIRE(legal_moves.size() == 1);
        CHECK(legal_moves[0].move().is_double_pawn());
    }

    SECTION("Blocked moves")
    {
        auto game_state = GameState::from_fen("8/8/8/8/8/2p5/2P5/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        CHECK(moves.size() == 0);
    }

    SECTION("Promoting moves")
    {
        auto game_state = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8").value();
        auto legal_moves = game_state.move_set().legal_moves();

        auto count = std::count_if(legal_moves.begin(), legal_moves.end(), [](const LegalMove& legal_move) {
            return legal_move->is_promotion();
        });

        REQUIRE(count == 4);
    }
}

TEST_CASE("Knight move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Attacking squares", "[comptime]")
    {
        auto attacks = attack_maps::generate_knight_attacks(Location::B1);

        std::array<Location, 3> expected = {
            Location::A3,
            Location::C3,
            Location::D2,
        };

        CHECK(attacks == BitBoard::from(expected));
    }

    SECTION("Attacking moves")
    {
        auto game_state = GameState::from_fen("8/8/8/3p4/1N6/p7/P7/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        REQUIRE(moves.size() == 5);
    }
}

TEST_CASE("King move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Attacking squares", "[comptime]")
    {
        auto attacks = attack_maps::generate_king_attacks(Location::D8);

        std::array<Location, 5> expected = {
            Location::D7,
            Location::E8,
            Location::E7,
            Location::C8,
            Location::C7,
        };

        CHECK(attacks == BitBoard::from(expected));
    }

    SECTION("Attacking moves")
    {
        auto game_state = GameState::from_fen("8/8/8/2p5/1KP5/pP6/P7/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        REQUIRE(moves.size() == 6);
    }
}
