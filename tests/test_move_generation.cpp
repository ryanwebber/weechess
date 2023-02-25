#include <span>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <weechess/comptime.h>
#include <weechess/game_state.h>
#include <weechess/move_generator.h>

// Simple perft implementation - this should be a command
uint64_t count_moves(weechess::GameState gs, int depth)
{
    if (depth == 0) {
        return 1;
    } else if (depth == 1) {
        return gs.move_set().legal_moves().size();
    }

    uint64_t count = 0;
    for (auto& move : gs.move_set().legal_moves()) {
        auto new_gs = weechess::GameState::by_performing_move(gs, move).value();
        count += count_moves(new_gs, depth - 1);
    }

    return count;
}

TEST_CASE("Legal move generation", "[rules]")
{
    using namespace weechess;

    // https://www.chessprogramming.org/Perft_Results#Position_5
    auto gs = GameState::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8").value();
    CHECK(count_moves(gs, 1) == 44);
    CHECK(count_moves(gs, 2) == 1486);
    CHECK(count_moves(gs, 3) == 62379);
    CHECK(count_moves(gs, 4) == 2103487);
    CHECK(count_moves(gs, 5) == 89941194);
}

TEST_CASE("Comptime knight attacks", "[movegen]")
{
    using namespace weechess;

    auto table = comptime::compute_knight_attacks();

    std::array<Location, 3> expected = {
        Location::A3,
        Location::C3,
        Location::D2,
    };

    CHECK(table[Location::B1.offset] == BitBoard::from(expected));
}

TEST_CASE("Comptime king attacks", "[movegen]")
{
    using namespace weechess;

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

TEST_CASE("Comptime pawn attacks", "[movegen]")
{
    using namespace weechess;

    auto table = comptime::compute_pawn_attacks();

    std::array<Location, 2> exp_1 = { Location::A3, Location::C3 };
    CHECK(table[Color::White][Location::B2.offset] == BitBoard::from(exp_1));

    std::array<Location, 2> exp_2 = { Location::C2, Location::E2 };
    CHECK(table[Color::Black][Location::D3.offset] == BitBoard::from(exp_2));

    CHECK(table[Color::Black][Location::C4.offset] != table[Color::White][Location::C4.offset]);
}

TEST_CASE("Comptime rook slide masks", "[movegen]")
{
    using namespace weechess;

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

TEST_CASE("Comptime bishop slide masks", "[movegen]")
{
    using namespace weechess;

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

TEST_CASE("Comptime unoptimized rook slides with blockers", "[movegen]")
{
    using namespace weechess;

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

TEST_CASE("Comptime unoptimized bishop slides with blockers", "[movegen]")
{
    using namespace weechess;

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

TEST_CASE("Optimized rook move sanity check", "[movegen]")
{
    using namespace weechess;

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

TEST_CASE("Optimized bishop move sanity check", "[movegen]")
{
    using namespace weechess;

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
