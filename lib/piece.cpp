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

Piece Piece::chromatic_inverse() const
{
    if (exists()) {
        return Piece(type(), invert_color(color()));
    } else {
        return *this;
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
}
