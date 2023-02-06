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

    std::string Piece::to_symbol() const {
        if (is(PieceType::Pawn)) {
            return is(Color::White) ? "♙" : "♟";
        } else if (is(PieceType::Knight)) {
            return is(Color::White) ? "♘" : "♞";
        } else if (is(PieceType::Bishop)) {
            return is(Color::White) ? "♗" : "♝";
        } else if (is(PieceType::Rook)) {
            return is(Color::White) ? "♖" : "♜";
        } else if (is(PieceType::Queen)) {
            return is(Color::White) ? "♕" : "♛";
        } else if (is(PieceType::King)) {
            return is(Color::White) ? "♔" : "♚";
        } else {
            return " ";
        }
    }
}
