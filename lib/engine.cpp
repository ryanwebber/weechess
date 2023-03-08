#include <weechess/engine.h>
#include <weechess/searcher.h>

#include "log.h"

namespace weechess {

Engine::Engine()
    : Engine(Settings())
{
}

Engine::Engine(const Settings& settings)
    : m_settings(settings)
{
}

SearchResult Engine::calculate(const GameState& game_state,
    const SearchParameters& parameters,
    const threading::Token& token,
    SearchDelegate& delegate) const
{

    // Some large number, doesn't really matter because we can never
    // reach it. If we do then good for us - we've beaten chess :)
    auto max_depth_to_search = parameters.max_depth.value_or(100000);

    auto time_start = std::chrono::high_resolution_clock::now();
    auto time_of_last_perf_event = time_start;
    SearchResult result;

    Searcher().search(game_state, max_depth_to_search, [&, this](const auto& progress, auto& control) {
        using namespace std::chrono;
        auto time_now = high_resolution_clock::now();
        auto time_elapsed = duration_cast<milliseconds>(time_now - time_start);
        auto time_since_last_perf_event = duration_cast<milliseconds>(time_now - time_of_last_perf_event);
        auto nodes_per_second = (1000 * progress.nodes_searched()) / time_elapsed.count();

        // Emit a performance event every so often
        if (time_since_last_perf_event > m_settings.perf_event_interval || progress.has_new_results()) {
            PerformanceEvent evt;
            evt.current_depth = progress.max_depth();
            evt.nodes_searched = progress.nodes_searched();
            evt.elapsed_time = time_elapsed;
            if (time_elapsed.count() != 0)
                evt.nodes_per_second = nodes_per_second;

            delegate.on_performance_event(evt);
            time_of_last_perf_event = time_now;
        }

        if (progress.has_new_results()) {
            EvaluationEvent evt;
            evt.best_line = progress.best_line();
            evt.evaluation = progress.evaluation();
            delegate.on_evaluation_event(evt);

            result.evaluation = evt.evaluation;
            result.best_line = evt.best_line;
        }

        // Update search control
        control.next_control_event = progress.nodes_searched() + 50000;

        auto invalidated = token.invalidated();
        auto reached_max_nodes = false;
        auto reached_max_time = false;

        if (parameters.max_nodes.has_value())
            reached_max_nodes = progress.nodes_searched() >= *parameters.max_nodes;

        if (parameters.max_search_time.has_value())
            reached_max_time = time_elapsed >= *parameters.max_search_time;

        control.stop = invalidated || reached_max_nodes || reached_max_time;
    });

    return result;
}

namespace {
    class DummySearchDelegate : public SearchDelegate { };
}

SearchResult Engine::calculate(const GameState& game_state, size_t depth)
{
    SearchParameters parameters;
    parameters.max_depth = depth;
    threading::Token token;
    DummySearchDelegate delegate;
    return Engine().calculate(game_state, parameters, token, delegate);
}

}
