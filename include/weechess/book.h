#pragma once

#include <span>
#include <unordered_map>
#include <vector>

#include <weechess/game_state.h>
#include <weechess/move.h>
#include <weechess/zobrist.h>

namespace weechess {

class Book {
public:
    using Table = std::unordered_map<zobrist::Hash, std::vector<Move>>;

    Book();
    Book(Table);

    std::span<const Move> lookup(const GameSnapshot&) const;

    static const Book default_instance;

private:
    Table m_lookup_table;
};

}
