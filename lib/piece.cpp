#include <weechess/piece.h>

namespace weechess {

Piece::Piece()
    : type(Type::None)
    , color(Color::White)
{
}

Piece::Piece(Type type, Color color)
    : type(type)
    , color(color)
{
}

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
