#pragma once

#include <optional>
#include <string>

namespace weechess {

enum class Color : uint8_t {
    White = 1 << 4,
    Black = 1 << 5,
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

    bool exists() const { return (representation & 0b111111) != 0; }

    char16_t to_symbol() const;

    static Piece none() { return Piece(); }
};

Color invert_color(Color);

template <typename T> class PieceMap : public std::array<T, 6> {
public:
    using std::array<T, 6>::array;

    T& operator[](Piece::Type type)
    {
        auto idx = static_cast<size_t>(type) & 0b111;
        return std::array<T, 6>::operator[](idx);
    }

    const T& operator[](Piece::Type type) const
    {
        auto idx = static_cast<size_t>(type) & 0b111;
        return std::array<T, 6>::operator[](idx);
    }
};
}
