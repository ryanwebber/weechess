#include <weechess/evaluator.h>

namespace weechess {

const Evaluator Evaluator::default_instance = Evaluator();

Evaluation Evaluator::evaluate(const GameState& state) const
{
    if (state.is_checkmate()) {
        return Evaluation::negative_inf();
    } else if (state.is_stalemate()) {
        return Evaluation { 0 };
    }

    Evaluation evaluation {};
    const auto& board = state.board();

    // Sum up the material value of each piece on the board for each color
    ColorMap<int> material_value { 0 };
    for (const auto& piece : Piece::all_valid_pieces) {
        const auto occupancy = board.occupancy_for(piece);
        material_value[piece.color] += Evaluation::piece_worth(piece.type) * occupancy.count();
    }

    evaluation.score += material_value[Color::White] - material_value[Color::Black];

    return state.turn_to_move() == Color::White ? evaluation : -evaluation;
}

}
