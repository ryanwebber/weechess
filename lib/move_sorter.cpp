#include <weechess/evaluator.h>
#include <weechess/move_sorter.h>

namespace weechess {

const MoveSorter MoveSorter::default_instance = MoveSorter();

Evaluation MoveSorter::evaluate(const LegalMove& legal_move) const
{
    const auto& move = legal_move.move();
    const auto& board = legal_move.snapshot().board;
    const auto& color = move.color();

    auto evaluation = Evaluation::zero();
    if (move.is_capture()) {
        evaluation += 10 * Evaluation::piece_worth(move.captured_piece_type())
            - Evaluation::piece_worth(move.moving_piece().type);
    }

    if (move.is_promotion()) {
        evaluation += Evaluation::piece_worth(move.promoted_piece_type());
    }

    if (board.pawn_attacks(invert_color(color))[move.end_location()]) {
        evaluation -= Evaluation::piece_worth(move.moving_piece().type);
    }

    return evaluation;
}

}
