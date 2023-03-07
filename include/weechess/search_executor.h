#pragma once

#include <chrono>
#include <optional>

#include <weechess/evaluator.h>
#include <weechess/searcher.h>
#include <weechess/threading.h>

namespace weechess {

struct PerformanceEvent {
    size_t current_depth;
    size_t nodes_searched;
    size_t nodes_per_second;
    std::chrono::duration<size_t, std::milli> elapsed_time;
};

struct EvaluationEvent {
    std::vector<Move> best_line;
    Evaluation evaluation;
};

class SearchDelegate {
public:
    SearchDelegate() = default;
    virtual ~SearchDelegate() = default;

    virtual void on_evaluation_event(const EvaluationEvent&) {};
    virtual void on_performance_event(const PerformanceEvent&) {};
};

struct SearchParameters {
    std::optional<size_t> max_depth {};
    std::optional<size_t> max_nodes {};
    std::optional<std::chrono::duration<size_t, std::milli>> max_search_time { std::chrono::seconds(10) };
};

struct SearchResult {
    Evaluation evaluation;
    std::vector<Move> best_line;
};

class SearchExecutor {
public:
    SearchExecutor(GameState, SearchParameters);
    SearchResult execute(SearchDelegate&, const threading::Token&);

    std::chrono::duration<size_t, std::milli> perf_event_interval() const { return m_perfEventInterval; }
    void set_perf_event_interval(std::chrono::duration<size_t, std::milli> interval) { m_perfEventInterval = interval; }

private:
    GameState m_game_state;
    SearchParameters m_parameters;

    std::chrono::duration<size_t, std::milli> m_perfEventInterval { 500 };
};

}
