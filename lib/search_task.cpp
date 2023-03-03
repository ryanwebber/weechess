#include <weechess/search_task.h>

namespace weechess {
SearchTask::SearchTask(SearchLimits limits)
    : m_limits(limits)
{
}

void SearchTask::execute(std::function<bool(SearchResult)> checkpoint)
{
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        auto should_continue = checkpoint(SearchResult());
        if (!should_continue)
            break;
    }
}

}
