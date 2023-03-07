#include <weechess/transposition_table.h>

namespace weechess {

void TranspositionTable::insert(const GameSnapshot& snapshot, const TranspositionTable::Value& entry)
{
    m_table.insert_or_assign(snapshot.zobrist_hash(), entry);
}

std::optional<TranspositionTable::Value> TranspositionTable::find(const GameSnapshot& snapshot) const
{
    auto itr = m_table.find(snapshot.zobrist_hash());
    if (itr == m_table.end())
        return {};
    return itr->second;
}

}
