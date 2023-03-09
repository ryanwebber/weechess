#include <weechess/book.h>

#include "generated/book_data.h"

namespace weechess {

Book::Book() = default;
Book::Book(Book::Data data)
    : m_data(data)
{
}

std::span<const Move> Book::lookup(const GameSnapshot& snapshot) const { return lookup(snapshot.zobrist_hash()); }
std::span<const Move> Book::lookup(const zobrist::Hash& hash) const
{
    auto it
        = std::lower_bound(m_data.entries.begin(), m_data.entries.end(), hash, [](const auto& entry, const auto& hash) {
              return entry.hash < hash;
          });

    if (it != m_data.entries.end() && it->hash == hash) {
        return { m_data.moves.begin() + it->offset, it->count };
    }

    return {};
}

const Book Book::default_instance(generated::book_data);

}
