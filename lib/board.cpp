#include <weechess/board.h>

namespace weechess {

Board::Board() = default;
Board::Board(Buffer cells)
    : m_cells(cells)
{
}

Piece Board::piece_at(Location location) const { return m_cells[location.offset]; }

Piece Board::set_piece_at(Location location, Piece piece)
{
    Piece old_piece(m_cells[location.offset]);
    m_cells[location.offset] = piece;
    return old_piece;
}

Color Board::color_at(Location location) const
{
    return (location.file() + location.rank()) % 2 == 0 ? Color::White : Color::Black;
}

std::span<const Piece> Board::cells() const { return m_cells; }

}
