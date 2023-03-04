#pragma once

#include <span>

#include <weechess/color_map.h>
#include <weechess/game_state.h>
#include <weechess/threading.h>

namespace weechess {

struct SearchQuery {
    std::span<const LegalMove> moves {};
};

struct SearchProgress {
    size_t current_depth { 0 };
    size_t nodes_searched { 0 };
};

struct SearchControl {
    bool stop { false };
    size_t next_control_event { 1 };
};

class SearchTask {
public:
    using Checkpointer = std::function<void(const SearchProgress&, SearchControl&)>;

    SearchTask() = default;
    void execute(const SearchQuery&, const Checkpointer&);
};

}
