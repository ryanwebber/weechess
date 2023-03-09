#pragma once

#include <weechess/game_state.h>
#include <weechess/move.h>
#include <weechess/piece.h>

namespace weechess {

constexpr std::array<int, 7> piece_values = {
    0,
    100,
    300,
    300,
    500,
    900,
    0,
};

struct Evaluation {
    int score;

    constexpr Evaluation invert() const { return { -score }; }
    constexpr bool operator<(const Evaluation& other) const { return score < other.score; }
    constexpr bool operator>(const Evaluation& other) const { return score > other.score; }
    constexpr bool operator<=(const Evaluation& other) const { return score <= other.score; }
    constexpr bool operator>=(const Evaluation& other) const { return score >= other.score; }
    constexpr bool operator==(const Evaluation& other) const { return score == other.score; }

    constexpr Evaluation operator-() const { return { -score }; }
    constexpr Evaluation operator+(const Evaluation& other) const { return { score + other.score }; }
    constexpr Evaluation operator-(const Evaluation& other) const { return { score - other.score }; }
    constexpr Evaluation operator+=(const Evaluation& other) { return { score += other.score }; }
    constexpr Evaluation operator-=(const Evaluation& other) { return { score -= other.score }; }
    constexpr Evaluation operator+=(const int& other) { return { score += other }; }
    constexpr Evaluation operator-=(const int& other) { return { score -= other }; }

    constexpr operator int() const { return score; }

    constexpr static Evaluation zero() { return { 0 }; }
    constexpr static Evaluation negative_inf() { return { -100 * piece_worth(Piece::Type::Pawn) }; }
    constexpr static Evaluation positive_inf() { return { +100 * piece_worth(Piece::Type::Pawn) }; }
    constexpr static Evaluation mate_in(size_t) { return positive_inf(); }

    constexpr static int piece_worth(Piece::Type type) { return piece_values[static_cast<int>(type)]; }
    constexpr static int pawns(int i) { return piece_values[static_cast<int>(Piece::Type::Pawn)] * i; }
};

class Evaluator {
public:
    Evaluator() = default;

    // Evaluate the given game state
    Evaluation evaluate(const GameState&) const;
    Evaluation operator()(const GameState& state) const { return evaluate(state); }

    static const Evaluator default_instance;
};

}
