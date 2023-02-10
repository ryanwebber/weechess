#pragma once

#include <optional>
#include <string>

namespace weechess {

    enum class Color: uint8_t {
        White   = 1 << 6,
        Black   = 1 << 7,        
    };

    struct Piece {

        enum class Type: uint8_t {
            None    = 0,
            Pawn    = 1 << 0,
            Knight  = 1 << 1,
            Bishop  = 1 << 2,
            Rook    = 1 << 3,
            Queen   = 1 << 4,
            King    = 1 << 5,
        };

        static constexpr uint8_t to_index(Type type) {
            uint8_t result;
            uint8_t x = static_cast<uint8_t>(type);
            while (x >> 1) {
                result++;
            }

            return result;
        }

        uint8_t representation;

        Piece();
        Piece(Type type, Color color);

        bool is(Type type) const;
        bool is(Color color) const;

        bool exists() const {
            return (representation & 0b111111) != 0;
        }

        char16_t to_symbol() const;

        static Piece none() {
            return Piece();
        }
    };

    Color invert_color(Color);
}
