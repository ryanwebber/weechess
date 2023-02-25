#include <weechess/board.h>

namespace weechess {

size_t piece_index(Piece piece)
{
    auto offset = piece.color() == Color::White ? 0 : 6;
    return offset + static_cast<size_t>(piece.type());
}

Board::Board() = default;
Board::Board(Buffer occupancy)
    : m_occupancy(occupancy)
{
}

Piece Board::piece_at(Location location) const
{
    for (auto i = 0; i < m_occupancy.size(); i++) {
        if (m_occupancy[i][location]) {
            auto color = i < 6 ? Color::White : Color::Black;
            auto type = static_cast<Piece::Type>(i % 6);
            return Piece(type, color);
        }
    }

    return Piece::none();
}

Color Board::color_at(Location location) const
{
    return (location.file() + location.rank()) % 2 == 0 ? Color::White : Color::Black;
}

BitBoard& Board::occupancy_for(Piece piece) { return m_occupancy[piece_index(piece)]; }
const BitBoard& Board::occupancy_for(Piece piece) const { return m_occupancy[piece_index(piece)]; }

std::array<Piece, 64> Board::to_array() const
{
    std::array<Piece, 64> pieces {};
    for (auto i = 0; i < 64; i++) {
        Location location(i);
        pieces[i] = piece_at(location);
    }

    return pieces;
}

Board::Builder& Board::Builder::set_piece(const Piece& piece, const Location& location)
{
    m_pieces[location.offset] = piece;
    return *this;
}

Piece& Board::Builder::operator[](const Location& location) { return m_pieces[location.offset]; }
const Piece& Board::Builder::operator[](const Location& location) const { return m_pieces[location.offset]; }

Board Board::Builder::build() const
{
    Board::Buffer occupancy {};

    for (auto i = 0; i < 64; i++) {
        Location location(i);
        Piece piece = m_pieces[i];
        auto index = piece_index(piece);

        occupancy[index].set(location);
    }

    return Board(occupancy);
}

}
