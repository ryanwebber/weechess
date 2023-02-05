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

    std::optional<char> Piece::to_fen() const {
        if (is(PieceType::Pawn)) {
            return is(Color::White) ? 'P' : 'p';
        } else if (is(PieceType::Knight)) {
            return is(Color::White) ? 'N' : 'n';
        } else if (is(PieceType::Bishop)) {
            return is(Color::White) ? 'B' : 'b';
        } else if (is(PieceType::Rook)) {
            return is(Color::White) ? 'R' : 'r';
        } else if (is(PieceType::Queen)) {
            return is(Color::White) ? 'Q' : 'q';
        } else if (is(PieceType::King)) {
            return is(Color::White) ? 'K' : 'k';
        } else {
            return std::nullopt;
        }
    }

    std::optional<Piece> Piece::from_fen(char c) {
        switch (c) {
            case 'P': return Piece(PieceType::Pawn, Color::White);
            case 'N': return Piece(PieceType::Knight, Color::White);
            case 'B': return Piece(PieceType::Bishop, Color::White);
            case 'R': return Piece(PieceType::Rook, Color::White);
            case 'Q': return Piece(PieceType::Queen, Color::White);
            case 'K': return Piece(PieceType::King, Color::White);
            case 'p': return Piece(PieceType::Pawn, Color::Black);
            case 'n': return Piece(PieceType::Knight, Color::Black);
            case 'b': return Piece(PieceType::Bishop, Color::Black);
            case 'r': return Piece(PieceType::Rook, Color::Black);
            case 'q': return Piece(PieceType::Queen, Color::Black);
            case 'k': return Piece(PieceType::King, Color::Black);
            default: return std::nullopt;
        }
    }
}
