#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <string>

namespace weechess {

enum class Color : uint8_t {
    White = 1 << 3,
    Black = 1 << 4,
};

constexpr std::array<Color, 2> all_colors = { Color::White, Color::Black };

struct Piece {

    constexpr static uint8_t type_mask = 0b00111;
    constexpr static uint8_t color_mask = 0b11000;
    enum class Type : uint8_t {
        None = 0,
        Pawn = 1,
        Knight = 2,
        Bishop = 3,
        Rook = 4,
        Queen = 5,
        King = 6,
    };

    static constexpr std::array<Type, 6> types = {
        Type::Pawn,
        Type::Knight,
        Type::Bishop,
        Type::Rook,
        Type::Queen,
        Type::King,
    };

    Type type;
    Color color;

    Piece();
    Piece(Type type, Color color);

    bool is(Type) const;
    bool is(Color) const;

    bool exists() const;
    bool is_none() const;

    char16_t to_symbol() const;
    char to_letter() const;

    bool operator==(const Piece& other) const;

    static Piece none() { return Piece(); }
    static const std::array<Piece, 12> all_valid_pieces;
};

Color invert_color(Color);

inline Piece::Piece()
    : type(Type::None)
    , color(Color::White)
{
}

inline Piece::Piece(Type type, Color color)
    : type(type)
    , color(color)
{
}

inline bool Piece::is(Type t) const { return type == t; }
inline bool Piece::is(Color c) const { return color == c; };
inline bool Piece::exists() const { return type != Type::None; }
inline bool Piece::is_none() const { return type == Type::None; }
inline bool Piece::operator==(const Piece& other) const { return other.type == type && other.color == color; };

std::ostream& operator<<(std::ostream&, const Piece&);

}
