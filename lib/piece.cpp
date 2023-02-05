#include "piece.h"

namespace weechess {

    Piece::Piece()
        : representation(0) {}

    Piece::Piece(PieceType type, Color color) {
        representation = static_cast<uint8_t>(type) | static_cast<uint8_t>(color);
    }

    bool Piece::is(PieceType type) const {
        return (representation & static_cast<uint8_t>(type)) != 0;
    }

    bool Piece::is(Color color) const {
        return (representation & static_cast<uint8_t>(color)) != 0;
    }
}
