#include <weechess/search_executor.h>
#include <weechess/search_task.h>

namespace weechess {

SearchExecutor::SearchExecutor(GameState gameState, SearchParameters parameters)
    : m_gameState(gameState)
    , m_parameters(parameters)
{
}

SearchResult SearchExecutor::execute(SearchDelegate& delagate, const threading::Token& token)
{
    SearchQuery query;
    query.moves = m_gameState.move_set().legal_moves();

    auto time_start = std::chrono::high_resolution_clock::now();
    auto time_of_last_perf_event = time_start;

    SearchTask().execute(query, [&, this](const auto& progress, auto& control) {
        using namespace std::chrono;
        auto time_now = high_resolution_clock::now();
        auto time_elapsed = duration_cast<milliseconds>(time_now - time_start);
        auto time_since_last_perf_event = duration_cast<milliseconds>(time_now - time_of_last_perf_event);
        auto nodes_per_second = 1000 * (progress.nodes_searched / time_elapsed.count());

        // Emit a performance event every so often
        if (time_since_last_perf_event > this->m_perfEventInterval) {
            PerformanceEvent evt;
            evt.current_depth = progress.current_depth;
            evt.nodes_searched = progress.nodes_searched;
            evt.elapsed_time = time_elapsed;
            if (time_elapsed.count() != 0)
                evt.nodes_per_second = nodes_per_second;

            delagate.on_performance_event(evt);
            time_of_last_perf_event = time_now;
        }

        // Update search control
        control.next_control_event = progress.nodes_searched + 100000;

        auto invalidated = token.invalidated();
        auto reached_max_depth
            = m_parameters.max_depth.has_value() && progress.current_depth >= *m_parameters.max_depth;
        auto reached_max_nodes
            = m_parameters.max_nodes.has_value() && progress.nodes_searched >= *m_parameters.max_nodes;
        auto reached_max_time
            = m_parameters.max_search_time.has_value() && time_elapsed >= *m_parameters.max_search_time;

        control.stop = invalidated || reached_max_depth || reached_max_nodes || reached_max_time;
    });

    return SearchResult();
}

}
