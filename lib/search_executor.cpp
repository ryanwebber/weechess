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

    SearchTask().execute(query, [&](const SearchProgress& progress) {
        delagate.on_performance_event({ 0, 0 });
        return token.invalidated() ? SearchContinuation::Stop : SearchContinuation::Continue;
    });

    return SearchResult();
}

}
