#include <weechess/transposition_table.h>

// #define WEECHESS_USE_TRANSPOSITIONS

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

std::optional<TranspositionTable::Value> TranspositionTable::lookup(
    const GameSnapshot& snapshot, size_t depth, size_t max_depth, Evaluation alpha, Evaluation beta) const
{
#ifdef WEECHESS_USE_TRANSPOSITIONS
    auto itr = m_table.find(snapshot.zobrist_hash());
    if (itr == m_table.end())
        return {};

    if ((itr->second.max_depth - itr->second.depth) < (max_depth - depth))
        return {};

    if (itr->second.type == TranspositionEntry::Type::Exact)
        return itr->second;
    else if (itr->second.type == TranspositionEntry::Type::LowerBound && itr->second.evaluation >= beta)
        return itr->second;
    else if (itr->second.type == TranspositionEntry::Type::UpperBound && itr->second.evaluation <= alpha)
        return itr->second;
    else
        return {};
#else
    return {};
#endif
}

}
