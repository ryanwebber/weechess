#pragma once

#include <optional>
#include <string>

namespace weechess {

enum class Color : uint8_t {
    White = 1 << 3,
    Black = 1 << 4,
};

struct Piece {

    enum class Type : uint8_t {
        None = 0,
        Pawn = 1,
        Knight = 2,
        Bishop = 3,
        Rook = 4,
        Queen = 5,
        King = 6,
    };

    uint8_t representation;

    Piece();
    Piece(Type type, Color color);

    bool is(Type type) const;
    bool is(Color color) const;

    Type type() const;
    Color color() const;

    bool exists() const { return (representation & 0b111) != 0; }
    bool is_none() const { return !exists(); }

    char16_t to_symbol() const;

    Piece chromatic_inverse() const;

    static Piece none() { return Piece(); }
};

Color invert_color(Color);

}
