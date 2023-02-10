#include <weechess/piece.h>

namespace weechess {

Piece::Piece()
    : representation(0)
{
}

Piece::Piece(Type type, Color color) { representation = static_cast<uint8_t>(type) | static_cast<uint8_t>(color); }

bool Piece::is(Type type) const { return (representation & static_cast<uint8_t>(type)) != 0; }

bool Piece::is(Color color) const { return (representation & static_cast<uint8_t>(color)) != 0; }

char16_t Piece::to_symbol() const
{
    if (is(Type::Pawn)) {
        return is(Color::White) ? u'♙' : u'♟';
    } else if (is(Type::Knight)) {
        return is(Color::White) ? u'♘' : u'♞';
    } else if (is(Type::Bishop)) {
        return is(Color::White) ? u'♗' : u'♝';
    } else if (is(Type::Rook)) {
        return is(Color::White) ? u'♖' : u'♜';
    } else if (is(Type::Queen)) {
        return is(Color::White) ? u'♕' : u'♛';
    } else if (is(Type::King)) {
        return is(Color::White) ? u'♔' : u'♚';
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
}
