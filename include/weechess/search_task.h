#include <weechess/threading.h>

namespace weechess {

struct SearchLimits { };

struct SearchResult { };

class SearchTask {
public:
    SearchTask(SearchLimits limits);
    void execute(std::function<bool(SearchResult)> checkpoint);

private:
    SearchLimits m_limits;
};

}
