#pragma once

#include <chrono>
#include <optional>
#include <random>

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
    Evaluation evaluation;
    std::vector<Move> best_line;
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
    bool is_book_move { false };
};

class Engine {
    struct Settings {
        unsigned int random_seed { std::random_device()() };
        std::chrono::duration<size_t, std::milli> perf_event_interval { 500 };
    };

public:
    Engine();
    Engine(const Settings&);

    Settings& settings() { return m_settings; }
    const Settings& settings() const { return m_settings; }

    SearchResult calculate(const GameState&, const SearchParameters&, const threading::Token&, SearchDelegate&);
    static SearchResult calculate(const GameState&, size_t depth);

private:
    Settings m_settings;
    std::default_random_engine m_random_engine;
};

}
