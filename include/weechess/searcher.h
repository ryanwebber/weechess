#pragma once

#include <span>
#include <vector>

#include <weechess/color_map.h>
#include <weechess/evaluator.h>
#include <weechess/game_state.h>
#include <weechess/threading.h>

namespace weechess {

class SearchInstance;

class SearchProgress {
public:
    SearchProgress(SearchInstance* search_instance, bool has_new_results, size_t max_depth_reached);

    bool has_new_results() const;
    size_t max_depth() const;
    size_t nodes_searched() const;
    Evaluation evaluation() const;
    std::vector<Move> best_line() const;

private:
    bool m_has_new_results;
    size_t m_max_depth_reached;
    SearchInstance* m_search_instance;
};

struct SearchControl {
    bool stop { false };
    size_t next_control_event { 1 };
};

class Searcher {
public:
    using Checkpointer = std::function<void(const SearchProgress&, SearchControl&)>;

    Searcher() = default;
    void search(const GameState& game_state, size_t max_depth, const Checkpointer&);
};

}
