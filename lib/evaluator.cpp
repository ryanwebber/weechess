#include <weechess/evaluator.h>

namespace weechess {

const Evaluator Evaluator::default_instance = Evaluator();

const unsigned int piece_values[] = {
    0, // None
    10, // Pawn
    30, // Knight
    30, // Bishop
    50, // Rook
    90, // Queen
    0, // King
};

Evaluation Evaluator::evaluate(const GameState& state) const
{
    Evaluation evaluation {};

    const auto& board = state.board();

    // Sum up the material value of each piece on the board for each color
    ColorMap<unsigned int> material_value { 0 };
    for (const auto& piece : Piece::all_valid_pieces) {
        const auto occupancy = board.occupancy_for(piece);
        material_value[piece.color] += piece_values[static_cast<int>(piece.type)] * occupancy.count();
    }

    evaluation.relative_score += material_value[Color::White] - material_value[Color::Black];

    return evaluation;
}

}
