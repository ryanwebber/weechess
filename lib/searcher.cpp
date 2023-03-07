#include <algorithm>
#include <iterator>
#include <optional>
#include <vector>

#include <weechess/evaluator.h>
#include <weechess/move_sorter.h>
#include <weechess/searcher.h>
#include <weechess/transposition_table.h>

#include "log.h"

namespace weechess {

using Checkpointer = std::function<void(const SearchProgress&, SearchControl&)>;

class SearchAbortedException : public std::exception { };

class SearchInstance {
private:
    size_t m_nodes_searched { 0 };
    size_t m_next_control_event { 0 };
    TranspositionTable m_transposition_table {};

    const Checkpointer& m_checkpointer;
    const GameState& m_root_game_state;

    void submit_progress(size_t depth, bool has_new_results)
    {
        SearchProgress progress(this, has_new_results, depth);
        SearchControl control;
        m_checkpointer(progress, control);

        if (control.stop)
            throw SearchAbortedException();

        m_next_control_event = control.next_control_event;
    }

    inline Evaluation search(
        const GameState& game_state, size_t depth, size_t max_depth, Evaluation alpha, Evaluation beta)
    {
        m_nodes_searched++;
        if (auto entry = m_transposition_table.lookup(game_state.snapshot(), depth, max_depth, alpha, beta);
            entry.has_value()) {
            return entry->evaluation;
        }

        if (depth >= max_depth) {
            // TODO: Perform quiescence search
            return Evaluator::default_instance.evaluate(game_state);
        }

        const auto& move_set = game_state.move_set();
        if (move_set.legal_moves().empty()) {
            // Don't bother searching further, the game is either in a
            // checkmate or stalemate
            return Evaluator::default_instance.evaluate(game_state);
        }

        auto evaluation_type = TranspositionEntry::Type::UpperBound;
        std::optional<Move> best_move {};

        std::array<LegalMove, 256> legal_moves;
        assert(move_set.legal_moves().size() <= legal_moves.size());

        // Sort the legal moves by a rough evaluation of move quality. This
        // improves Alpha-Beta pruning performance significantly since we're
        // likely to find good moves first, and thus prune more of the search
        std::copy(move_set.legal_moves().begin(), move_set.legal_moves().end(), legal_moves.begin());
        std::sort(legal_moves.begin(),
            std::next(legal_moves.begin(), move_set.legal_moves().size()),
            MoveSorter::default_instance);

        for (auto i = 0; i < move_set.legal_moves().size(); i++) {
            const auto& legal_move = legal_moves[i];
            assert(legal_move.move() != Move::null);

            auto new_game_state = GameState(legal_move.snapshot());
            auto evaluation = -search(new_game_state, depth + 1, max_depth, -beta, -alpha);

            // This move is better than a previous best-case for the opponent,
            // so the opponent won't allow us to make it. We can prune the rest of the
            // search tree.
            if (evaluation >= beta) {
                m_transposition_table.insert(game_state.snapshot(),
                    {
                        .depth = depth,
                        .max_depth = max_depth,
                        .evaluation = beta,
                        .move = legal_move.move(),
                        .type = TranspositionEntry::Type::LowerBound,
                    });

                return beta;
            }

            if (evaluation > alpha) {
                alpha = evaluation;
                best_move = legal_move.move();
                evaluation_type = TranspositionEntry::Type::Exact;
            }
        }

        if (best_move.has_value()) {
            m_transposition_table.insert(game_state.snapshot(),
                {
                    .depth = depth,
                    .max_depth = max_depth,
                    .evaluation = alpha,
                    .move = best_move.value(),
                    .type = evaluation_type,
                });
        }

        if (m_nodes_searched > m_next_control_event) {
            submit_progress(max_depth, false);
        }

        return alpha;
    }

public:
    SearchInstance(const Checkpointer& checkpointer, const GameState& root_game_state)
        : m_checkpointer(checkpointer)
        , m_root_game_state(root_game_state)
    {
    }

    void search_to_depth(size_t max_depth)
    {
        log::debug("Starting search to depth: {}", max_depth);
        search(m_root_game_state, 0, max_depth, Evaluation::negative_inf(), Evaluation::positive_inf());
        submit_progress(max_depth, true);
    }

    friend class SearchProgress;
};

SearchProgress::SearchProgress(SearchInstance* instance, bool has_new_results, size_t max_depth_reached)
    : m_has_new_results(has_new_results)
    , m_max_depth_reached(max_depth_reached)
    , m_search_instance(instance)
{
}

bool SearchProgress::has_new_results() const { return m_has_new_results; }
size_t SearchProgress::max_depth() const { return m_max_depth_reached; }
size_t SearchProgress::nodes_searched() const { return m_search_instance->m_nodes_searched; }

Evaluation SearchProgress::evaluation() const
{
    auto entry = m_search_instance->m_transposition_table.find(m_search_instance->m_root_game_state.snapshot());
    if (!entry.has_value()) {
        return Evaluation::zero();
    }

    return entry->evaluation;
}

std::vector<Move> SearchProgress::best_line() const
{
    std::vector<Move> line = {};

    std::optional<GameSnapshot> next_snapshot = m_search_instance->m_root_game_state.snapshot();
    while (next_snapshot.has_value() && line.size() < m_max_depth_reached) {
        auto entry = m_search_instance->m_transposition_table.find(next_snapshot.value());
        if (!entry.has_value()) {
            break;
        }

        line.push_back(entry->move);
        next_snapshot = next_snapshot->by_performing_move(entry->move);
    }

    return line;
}

void Searcher::search(const GameState& game_state, size_t max_depth, const Checkpointer& checkpointer)
{
    SearchInstance instance(checkpointer, game_state);
    if (game_state.move_set().legal_moves().empty()) {
        return;
    }

    // https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search
    for (size_t i = 0; i < max_depth; ++i) {
        try {
            instance.search_to_depth(i + 1);
        } catch (const SearchAbortedException&) {
            log::debug("Search aborted");
            break;
        }
    }

    log::debug("Search finished");
}

}
