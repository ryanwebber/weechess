#include <weechess/piece.h>

namespace weechess {

Piece::Piece()
    : representation(0)
{
}

Piece::Piece(Type type, Color color) { representation = static_cast<uint8_t>(type) | static_cast<uint8_t>(color); }

bool Piece::is(Type type) const { return this->type() == type; }

bool Piece::is(Color color) const { return exists() && ((representation & static_cast<uint8_t>(color)) != 0); }

Color Piece::color() const { return static_cast<Color>(representation & 0b00011000); }

Piece::Type Piece::type() const { return static_cast<Type>(representation & 0b00000111); }

char16_t Piece::to_symbol() const
{
    if (is(Type::Pawn)) {
        return is(Color::Black) ? u'♙' : u'♟';
    } else if (is(Type::Knight)) {
        return is(Color::Black) ? u'♘' : u'♞';
    } else if (is(Type::Bishop)) {
        return is(Color::Black) ? u'♗' : u'♝';
    } else if (is(Type::Rook)) {
        return is(Color::Black) ? u'♖' : u'♜';
    } else if (is(Type::Queen)) {
        return is(Color::Black) ? u'♕' : u'♛';
    } else if (is(Type::King)) {
        return is(Color::Black) ? u'♔' : u'♚';
    } else {
        return ' ';
    }
}

char Piece::to_letter() const
{
    if (is(Type::Pawn)) {
        return is(Color::Black) ? 'p' : 'P';
    } else if (is(Type::Knight)) {
        return is(Color::Black) ? 'n' : 'N';
    } else if (is(Type::Bishop)) {
        return is(Color::Black) ? 'b' : 'B';
    } else if (is(Type::Rook)) {
        return is(Color::Black) ? 'r' : 'R';
    } else if (is(Type::Queen)) {
        return is(Color::Black) ? 'q' : 'Q';
    } else if (is(Type::King)) {
        return is(Color::Black) ? 'k' : 'K';
    } else {
        return ' ';
    }
}

Color invert_color(Color color)
{
    if (color == Color::White) {
        return Color::Black;
    } else {
        return Color::White;
    }
}

const std::array<Piece, 12> Piece::all_valid_pieces = {
    Piece(Piece::Type::Pawn, Color::White),
    Piece(Piece::Type::Knight, Color::White),
    Piece(Piece::Type::Bishop, Color::White),
    Piece(Piece::Type::Rook, Color::White),
    Piece(Piece::Type::Queen, Color::White),
    Piece(Piece::Type::King, Color::White),
    Piece(Piece::Type::Pawn, Color::Black),
    Piece(Piece::Type::Knight, Color::Black),
    Piece(Piece::Type::Bishop, Color::Black),
    Piece(Piece::Type::Rook, Color::Black),
    Piece(Piece::Type::Queen, Color::Black),
    Piece(Piece::Type::King, Color::Black),
};

std::ostream& operator<<(std::ostream& os, const Piece& piece)
{
    os << piece.to_letter();
    return os;
}

}
