#include <weechess/search_executor.h>
#include <weechess/searcher.h>

#include "log.h"

namespace weechess {

SearchExecutor::SearchExecutor(GameState gameState, SearchParameters parameters)
    : m_game_state(gameState)
    , m_parameters(parameters)
{
}

SearchResult SearchExecutor::execute(SearchDelegate& delagate, const threading::Token& token)
{
    // Some large number, doesn't really matter because we can never
    // reach it. If we do then good for us - we've beaten chess :)
    auto max_depth_to_search = m_parameters.max_depth.value_or(100000);

    auto time_start = std::chrono::high_resolution_clock::now();
    auto time_of_last_perf_event = time_start;
    SearchResult result;

    Searcher().search(m_game_state, max_depth_to_search, [&, this](const auto& progress, auto& control) {
        using namespace std::chrono;
        auto time_now = high_resolution_clock::now();
        auto time_elapsed = duration_cast<milliseconds>(time_now - time_start);
        auto time_since_last_perf_event = duration_cast<milliseconds>(time_now - time_of_last_perf_event);
        auto nodes_per_second = (1000 * progress.nodes_searched()) / time_elapsed.count();

        // Emit a performance event every so often
        if (time_since_last_perf_event > this->m_perfEventInterval || progress.has_new_results()) {
            PerformanceEvent evt;
            evt.current_depth = progress.max_depth();
            evt.nodes_searched = progress.nodes_searched();
            evt.elapsed_time = time_elapsed;
            if (time_elapsed.count() != 0)
                evt.nodes_per_second = nodes_per_second;

            delagate.on_performance_event(evt);
            time_of_last_perf_event = time_now;
        }

        if (progress.has_new_results()) {
            EvaluationEvent evt;
            evt.best_line = progress.best_line();
            evt.evaluation = progress.evaluation();
            delagate.on_evaluation_event(evt);

            result.evaluation = evt.evaluation;
            result.best_line = evt.best_line;
        }

        // Update search control
        control.next_control_event = progress.nodes_searched() + 50000;

        auto invalidated = token.invalidated();
        auto reached_max_nodes
            = m_parameters.max_nodes.has_value() && progress.nodes_searched() >= *m_parameters.max_nodes;
        auto reached_max_time
            = m_parameters.max_search_time.has_value() && time_elapsed >= *m_parameters.max_search_time;

        control.stop = invalidated || reached_max_nodes || reached_max_time;
    });

    return result;
}

}
