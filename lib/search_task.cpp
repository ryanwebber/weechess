#include <vector>

#include <weechess/evaluator.h>
#include <weechess/search_task.h>

namespace weechess {

using Checkpointer = std::function<void(const SearchProgress&, SearchControl&)>;

inline bool recursive_search(size_t depth,
    size_t max_depth,
    std::span<const LegalMove> moves,
    std::vector<Move>& line,
    size_t& next_control_event,
    size_t& nodes_searched,
    const SearchTask::Checkpointer& checkpointer)
{
    for (const auto& legal_move : moves) {
        line.push_back(legal_move.move());
        GameState next_state(legal_move.snapshot());

        // TODO: Evaluation
        auto evaluation = Evaluator::default_instance.evaluate(next_state);
        (void)evaluation;

        nodes_searched++;

        if (depth < max_depth) {
            auto stop = recursive_search(depth + 1,
                max_depth,
                next_state.move_set().legal_moves(),
                line,
                next_control_event,
                nodes_searched,
                checkpointer);

            if (stop)
                return true;
        }

        line.pop_back();
    }

    if (nodes_searched > next_control_event) {
        SearchProgress progress;
        progress.current_depth = depth;
        progress.nodes_searched = nodes_searched;

        SearchControl control;
        control.stop = false;

        checkpointer(progress, control);

        if (control.stop)
            return true;

        next_control_event = control.next_control_event;
    }

    return false;
}

void SearchTask::execute(const SearchQuery& query, const Checkpointer& checkpoint)
{
    size_t nodes_searched = 0;
    size_t next_control_event = 0;
    std::vector<Move> line;

    // https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search
    for (size_t i = 0; i < std::numeric_limits<size_t>::max(); ++i) {
        auto stop = recursive_search(0, i, query.moves, line, next_control_event, nodes_searched, checkpoint);
        if (stop)
            break;
    }
}

}
