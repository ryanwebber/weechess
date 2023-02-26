#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/comptime.h>
#include <weechess/game_state.h>
#include <weechess/move_generator.h>

// Simple perft implementation - this should be a command
uint64_t do_perft(weechess::GameState gs, int depth, bool print = false)
{
    if (depth == 0) {
        return 1;
    } else if (depth == 1) {
        return gs.move_set().legal_moves().size();
    }

    uint64_t count = 0;
    for (const auto& move : gs.move_set().legal_moves()) {
        auto new_gs = weechess::GameState::by_performing_move(gs, move).value();
        auto result = do_perft(new_gs, depth - 1);
        count += result;

        if (print) {
            // UNSCOPED_INFO("Perft[" << depth << "] = " << result << "\n" << gs.verbose_description(move) << "\n\n");
            UNSCOPED_INFO(gs.san_notation(move) << ": " << result << "\n");
        }
    }

    return count;
}

TEST_CASE("Perft move generation counts", "[movegen]")
{
    using namespace weechess;

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

    SECTION("Slide masks", "[comptime]")
    {
        auto table = comptime::compute_rook_slide_masks();

        std::array<Location, 10> expectated = {
            // Left
            Location::C3,
            Location::B3,

            // Right
            Location::E3,
            Location::F3,
            Location::G3,

            // Up
            Location::D4,
            Location::D5,
            Location::D6,
            Location::D7,

            // Down
            Location::D2,
        };

        CHECK(table[Location::D3.offset] == BitBoard::from(expectated));
    }

    SECTION("Slides with blockers", "[comptime]")
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
        auto actual_moves = comptime::compute_rook_attacks_unoptimized(location, blockers);

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

        CHECK(actual_moves == BitBoard::from(expectated));
    }

    SECTION("Optimized movement sanity check", "[comptime]")
    {
        auto noise = 0xa2f8df60487ed0b0ULL;

        for (auto i = 0; i < 64; i++) {
            Location l(i);

            BitBoard occupancy;
            for (auto x = 0; x < 10; x++) {
                // Try to create slightly different board
                // layouts for each case, setting 10-ish bits
                occupancy.set(((noise ^ i) >> x) % 64);
            }

            occupancy.unset(l);

            INFO("BitBoard at " << l.to_string() << " with occupancy (" << occupancy.data() << "ULL)\n" << occupancy);

            auto slow_results = comptime::compute_rook_attacks_unoptimized(l, occupancy);
            auto fast_results = testapi::rook_attacks(l, occupancy);

            CHECK(slow_results == fast_results);
        }
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

    SECTION("Slide masks")
    {
        auto table = comptime::compute_bishop_slide_masks();

        std::array<Location, 9> expectated = {
            // Up-Left
            Location::C5,
            Location::B6,

            // Up-Right
            Location::E5,
            Location::F6,
            Location::G7,

            // Down-Left
            Location::C3,
            Location::B2,

            // Down-Right
            Location::E3,
            Location::F2,
        };

        CHECK(table[Location::D4.offset] == BitBoard::from(expectated));
    }

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
        auto actual_moves = comptime::compute_bishop_attacks_unoptimized(location, blockers);

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

        CHECK(actual_moves == BitBoard::from(expectated));
    }

    SECTION("Optimized movement sanity check", "[comptime]")
    {
        auto noise = 0xa2f8df60487ed0b0ULL;

        for (auto i = 0; i < 64; i++) {
            Location l(i);

            BitBoard occupancy;
            for (auto x = 0; x < 10; x++) {
                // Try to create slightly different board
                // layouts for each case, setting 10-ish bits
                occupancy.set(((noise ^ i) >> x) % 64);
            }

            occupancy.unset(l);

            INFO("BitBoard at " << l.to_string() << " with occupancy (" << occupancy.data() << "ULL)\n" << occupancy);

            auto slow_results = comptime::compute_bishop_attacks_unoptimized(l, occupancy);
            auto fast_results = testapi::bishop_attacks(l, occupancy);

            CHECK(slow_results == fast_results);
        }
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
        auto table = comptime::compute_pawn_attacks();

        std::array<Location, 2> exp_1 = { Location::A3, Location::C3 };
        CHECK(table[Color::White][Location::B2.offset] == BitBoard::from(exp_1));

        std::array<Location, 2> exp_2 = { Location::C2, Location::E2 };
        CHECK(table[Color::Black][Location::D3.offset] == BitBoard::from(exp_2));

        CHECK(table[Color::Black][Location::C4.offset] != table[Color::White][Location::C4.offset]);
    }

    SECTION("Capturing moves")
    {
        auto game_state = GameState::from_fen("8/8/8/8/8/1nr5/2P5/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();

        REQUIRE(moves.size() == 1);
        CHECK(moves[0].start_location() == Location::C2);
        CHECK(moves[0].end_location() == Location::B3);
        CHECK(moves[0].is_capture());
        CHECK(moves[0].captured_piece_type() == Piece::Type::Knight);
    }

    SECTION("Double moves")
    {
        auto game_state = GameState::from_fen("8/8/8/8/8/8/2P5/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();

        auto dbl_move = std::find_if(moves.begin(), moves.end(), [](const Move& m) {
            ;
            return m.end_location() == Location::C4;
        });

        REQUIRE(dbl_move != moves.end());
        CHECK(dbl_move->is_double_pawn());
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
        auto moves = game_state.move_set().legal_moves();

        auto count = std::count_if(moves.begin(), moves.end(), [](const Move& m) {
            ;
            return m.is_promotion();
        });

        REQUIRE(count == 4);
    }
}

TEST_CASE("Knight move generation", "[movegen]")
{
    using namespace weechess;

    SECTION("Attacking squares", "[comptime]")
    {
        auto table = comptime::compute_knight_attacks();

        std::array<Location, 3> expected = {
            Location::A3,
            Location::C3,
            Location::D2,
        };

        CHECK(table[Location::B1.offset] == BitBoard::from(expected));
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
        auto table = comptime::compute_king_attacks();

        std::array<Location, 5> expected = {
            Location::D7,
            Location::E8,
            Location::E7,
            Location::C8,
            Location::C7,
        };

        CHECK(table[Location::D8.offset] == BitBoard::from(expected));
    }

    SECTION("Attacking moves")
    {
        auto game_state = GameState::from_fen("8/8/8/2p5/1KP5/pP6/P7/8 w - - 0 1").value();
        auto moves = game_state.move_set().legal_moves();
        REQUIRE(moves.size() == 6);
    }
}
