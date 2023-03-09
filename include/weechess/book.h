#pragma once

#include <span>
#include <vector>

#include <weechess/game_state.h>
#include <weechess/move.h>
#include <weechess/zobrist.h>

namespace weechess {

class Book {
public:
    struct Entry {
        zobrist::Hash hash;
        size_t offset;
        size_t count;
    };

    struct Data {
        std::span<const Entry> entries;
        std::span<const Move> moves;
    };

    Book();
    Book(Data data);

    std::span<const Move> lookup(const GameSnapshot&) const;
    std::span<const Move> lookup(const zobrist::Hash&) const;

    static const Book default_instance;

private:
    Data m_data;
};

inline bool operator<(const Book::Entry& lhs, const Book::Entry& rhs) { return lhs.hash < rhs.hash; }

}
