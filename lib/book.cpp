#include <weechess/book.h>

#include "generated/book_data.h"

namespace weechess {

Book::Book() = default;
Book::Book(Book::Table lookup_table)
    : m_lookup_table(std::move(lookup_table))
{
}

std::span<const Move> Book::lookup(const GameSnapshot& snapshot) const
{
    if (auto it = m_lookup_table.find(snapshot.zobrist_hash()); it != m_lookup_table.end()) {
        return it->second;
    }

    return {};
}

const Book default_instance(generated::book_data);

}
