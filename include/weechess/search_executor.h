#pragma once

#include <weechess/search_task.h>
#include <weechess/threading.h>

namespace weechess {

struct PerformanceEvent {
    size_t depth;
    size_t positions_searched;
};

class SearchDelegate {
public:
    SearchDelegate() = default;
    virtual ~SearchDelegate() = default;

    virtual void on_best_move_changed(std::span<const Move>) {};
    virtual void on_performance_event(const PerformanceEvent&) {};
};

struct SearchParameters { };
struct SearchResult {
    std::optional<Move> best_move {};
};

class SearchExecutor {
public:
    SearchExecutor(GameState, SearchParameters);
    SearchResult execute(SearchDelegate&, const threading::Token&);

private:
    GameState m_gameState;
    SearchParameters m_parameters;
};

}
