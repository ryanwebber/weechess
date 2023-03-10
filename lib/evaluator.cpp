#include <functional>

#include <weechess/evaluator.h>

namespace weechess {

namespace {
    // clang-format off
    constexpr std::array<int, 64> pawns = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    constexpr std::array<int, 64> knights = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };

    constexpr std::array<int, 64> bishops = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };

    constexpr std::array<int, 64> rooks = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    };

    constexpr std::array<int, 64> queens = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    constexpr std::array<int, 64> kingMiddle = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    };

    constexpr std::array<int, 64> kingEnd = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };
    // clang-format on
}

const Evaluator Evaluator::default_instance = Evaluator();

struct EvaluationParameters {
    float normalized_end_game_weight;
};

template <typename... Args>
Evaluation reduce(const GameState& game_state, const EvaluationParameters& params, const Args&&... args)
{
    return (... + ([&](const auto& e) { return e(game_state, params); })(args));
}

// Sum up the material value of each piece on the board for each color
struct MaterialEvaluator {
    Evaluation operator()(const GameState& game_state, const EvaluationParameters&) const
    {
        const auto& board = game_state.board();

        ColorMap<int> material_value { 0 };
        for (const auto& piece : Piece::all_valid_pieces) {
            const auto occupancy = board.occupancy_for(piece);
            material_value[piece.color] += Evaluation::piece_worth(piece.type) * occupancy.count();
        }

        auto evaluation = Evaluation { material_value[Color::White] - material_value[Color::Black] };
        return game_state.turn_to_move() == Color::White ? evaluation : evaluation.invert();
    }
};

// If there aren't too many pieces left on the board and we have a winning advantage,
// we should try to force the king to the edge of the board
struct ForceKingToEdgeEvaluator {
    Evaluation operator()(const GameState& game_state, const EvaluationParameters& params) const
    {
        if (params.normalized_end_game_weight < 0.5f)
            return { 0 };

        auto white_piece_count = game_state.board().color_occupancy()[Color::White].count();
        auto black_piece_count = game_state.board().color_occupancy()[Color::Black].count();

        if (white_piece_count < black_piece_count + 2)
            return { 0 };

        auto white_king_location = game_state.board().occupancy_for(Piece(Piece::Type::King, Color::White)).lsb();
        auto black_king_location = game_state.board().occupancy_for(Piece(Piece::Type::King, Color::Black)).lsb();
        if (!white_king_location.has_value() || !black_king_location.has_value())
            return { 0 };

        int kings_distance = white_king_location->manhatthan_distance_to(*black_king_location);
        int edge_to_black_king_distance = black_king_location->distance_to_nearest_rank_edge()
            + black_king_location->distance_to_nearest_file_edge();

        int absolute_evaluation = ((10 * edge_to_black_king_distance) - kings_distance);
        auto evaluation = Evaluation { static_cast<int>(absolute_evaluation * params.normalized_end_game_weight) };
        return game_state.turn_to_move() == Color::White ? evaluation : evaluation.invert();
    }
};

struct GoodSquaresForPiecesEvaluator {
    Evaluation operator()(const GameState& game_state, const EvaluationParameters& params) const
    {
        auto evaluation = Evaluation::zero();
        auto color = game_state.turn_to_move();
        for (const auto type : Piece::types) {
            auto occupancy = game_state.board().occupancy_for(Piece(type, color));
            while (occupancy.any()) {
                auto location = occupancy.pop_lsb();
                auto board_index = location->offset;
                if (color == Color::White) {
                    auto file = location->file();
                    auto rank = 7 - location->rank();
                    board_index = rank * 8 + file;
                }

                switch (type) {
                case Piece::Type::Pawn:
                    evaluation += pawns[board_index];
                    break;
                case Piece::Type::Knight:
                    evaluation += knights[board_index];
                    break;
                case Piece::Type::Bishop:
                    evaluation += bishops[board_index];
                    break;
                case Piece::Type::Rook:
                    evaluation += rooks[board_index];
                    break;
                case Piece::Type::Queen:
                    evaluation += queens[board_index];
                    break;
                case Piece::Type::King:
                    if (params.normalized_end_game_weight < 0.5f)
                        evaluation += kingMiddle[board_index];
                    else
                        evaluation += kingEnd[board_index];

                    break;
                default:
                    break;
                }
            }
        }

        return evaluation;
    }
};

float compute_normalized_end_game_weight(const GameState& game_state)
{
    const auto& board = game_state.board();
    auto count_pieces = [&](Piece::Type type) {
        return board.occupancy_for(Piece(type, Color::White)).count()
            + board.occupancy_for(Piece(type, Color::Black)).count();
    };

    // Considerng how many pawns are left on the board
    auto w1 = 3.0f;
    auto v1 = static_cast<float>(count_pieces(Piece::Type::Pawn)) / 16.0f;

    // Considering if the queens are still on the board
    auto w2 = 1.0f;
    auto v2 = static_cast<float>(count_pieces(Piece::Type::Queen)) / 2.0f;

    // Considering how many total pieces are on the board
    auto w3 = 1.0f;
    auto v3 = static_cast<float>(board.shared_occupancy().count()) / 32.0f;

    return 1.0f - (w1 * v1 + w2 * v2 + w3 * v3) / (w1 + w2 + w3);
}

Evaluation Evaluator::evaluate(const GameState& state) const
{
    if (state.is_checkmate()) {
        return Evaluation::negative_inf();
    } else if (state.is_stalemate()) {
        return Evaluation { 0 };
    }

    EvaluationParameters parameters = {
        .normalized_end_game_weight = compute_normalized_end_game_weight(state),
    };

    // clang-format off
    return reduce(state, parameters,
        MaterialEvaluator(),
        ForceKingToEdgeEvaluator(),
        GoodSquaresForPiecesEvaluator()
    );
    // clang-format on
}
}
