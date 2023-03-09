#include <catch2/catch_test_macros.hpp>

#include <weechess/book.h>

TEST_CASE("Searching position in a book", "[search]")
{
    using namespace weechess;

    // From the book data
    auto hash = 5575041534985550402ULL;
    size_t expected_move_count = 6;

    auto moves = Book::default_instance.lookup(hash);
    REQUIRE(moves.size() == expected_move_count);
    REQUIRE(moves[0] == Move(Move::Data(268447058UL)));
}
