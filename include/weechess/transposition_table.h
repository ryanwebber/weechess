#pragma once

#include <unordered_map>

#include <weechess/evaluator.h>
#include <weechess/move.h>
#include <weechess/zobrist.h>

namespace weechess {

struct GameSnapshot;

struct TranspositionEntry {
    enum class Type {
        Exact,
        LowerBound,
        UpperBound,
    };

    Type type;
    Move move;
    size_t depth;
    size_t max_depth;
    Evaluation evaluation;
};

class TranspositionTable {
public:
    using Key = zobrist::Hash;
    using Value = TranspositionEntry;

    TranspositionTable() = default;

    void insert(const GameSnapshot&, const Value&);
    std::optional<Value> find(const GameSnapshot&) const;

private:
    std::unordered_map<Key, Value> m_table;
};

}
