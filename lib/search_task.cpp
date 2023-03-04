#include <weechess/search_task.h>

namespace weechess {

void SearchTask::execute(const SearchQuery& query, std::function<SearchContinuation(const SearchProgress&)> checkpoint)
{
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        SearchProgress progress;
        auto continuation = checkpoint(progress);
        if (continuation == SearchContinuation::Stop)
            break;
    }
}

}
