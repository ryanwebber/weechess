#pragma once

#include <optional>

namespace weechess {

    enum class PieceType: uint8_t {
        None = 0,
        Pawn    = 1 << 0,
        Knight  = 1 << 1,
        Bishop  = 1 << 2,
        Rook    = 1 << 3,
        Queen   = 1 << 4,
        King    = 1 << 5,
    };

    enum class Color: uint8_t {
        White   = 1 << 6,
        Black   = 1 << 7,        
    };

    struct Piece {
        uint8_t representation;

        Piece();
        Piece(PieceType type, Color color);

        bool is(PieceType type) const;
        bool is(Color color) const;

        bool exists() const {
            return is(PieceType::None);
        }

        static Piece none() {
            return Piece();
        }
    };

}
