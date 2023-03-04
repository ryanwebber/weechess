#pragma once

#include <span>

#include <weechess/color_map.h>
#include <weechess/game_state.h>
#include <weechess/threading.h>

namespace weechess {

struct SearchQuery {
    std::span<const LegalMove> moves {};
};

struct SearchProgress { };

enum class SearchContinuation {
    Continue,
    Stop,
};

class SearchTask {
public:
    SearchTask() = default;
    void execute(const SearchQuery&, std::function<SearchContinuation(const SearchProgress&)> checkpoint);

private:
    SearchQuery m_query;
};

}
