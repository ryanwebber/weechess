#pragma once

#include <chrono>
#include <optional>

#include <weechess/search_task.h>
#include <weechess/threading.h>

namespace weechess {

struct PerformanceEvent {
    size_t current_depth;
    size_t nodes_searched;
    size_t nodes_per_second;
    std::chrono::duration<size_t, std::milli> elapsed_time;
};

class SearchDelegate {
public:
    SearchDelegate() = default;
    virtual ~SearchDelegate() = default;

    virtual void on_best_move_changed(std::span<const Move>) {};
    virtual void on_performance_event(const PerformanceEvent&) {};
};

struct SearchParameters {
    std::optional<size_t> max_depth {};
    std::optional<size_t> max_nodes {};
    std::optional<std::chrono::duration<size_t, std::milli>> max_search_time {};
};

struct SearchResult {
    std::optional<Move> best_move {};
};

class SearchExecutor {
public:
    SearchExecutor(GameState, SearchParameters);
    SearchResult execute(SearchDelegate&, const threading::Token&);

    std::chrono::duration<size_t, std::milli> perf_event_interval() const { return m_perfEventInterval; }
    void set_perf_event_interval(std::chrono::duration<size_t, std::milli> interval) { m_perfEventInterval = interval; }

private:
    GameState m_gameState;
    SearchParameters m_parameters;

    std::chrono::duration<size_t, std::milli> m_perfEventInterval { 1000 };
};

}