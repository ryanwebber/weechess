#include <weechess/attack_maps.h>
#include <weechess/board.h>

namespace weechess {

inline size_t piece_index(Piece piece)
{
    auto offset = piece.color == Color::White ? 0 : 7;
    return offset + static_cast<size_t>(piece.type);
}

BitBoard& Board::Buffer::occupancy_for(Piece piece) { return m_occupancy[piece_index(piece)]; }
const BitBoard& Board::Buffer::occupancy_for(Piece piece) const { return m_occupancy[piece_index(piece)]; }

Board::Board() = default;
Board::Board(Buffer piece_buffer)
    : m_piece_buffer(std::move(piece_buffer))
    , m_shared_occupancy({})
    , m_color_occupancy({}, {})
{
    for (const auto& piece : Piece::all_valid_pieces) {
        const auto& occupancy = m_piece_buffer.occupancy_for(piece);
        m_shared_occupancy |= occupancy;
        m_color_occupancy[piece.color] |= occupancy;
    }
}

Piece Board::piece_at(Location location) const
{
    for (const auto& piece : Piece::all_valid_pieces) {
        if (m_piece_buffer.occupancy_for(piece)[location.offset])
            return piece;
    }

    return Piece::none();
}

Color Board::color_at(Location location) const
{
    return (location.file() + location.rank()) % 2 == 0 ? Color::White : Color::Black;
}

const Board::Buffer& Board::piece_buffer() const { return m_piece_buffer; }
const BitBoard& Board::occupancy_for(Piece piece) const { return m_piece_buffer.occupancy_for(piece); }
const BitBoard& Board::shared_occupancy() const { return m_shared_occupancy; }
const ColorMap<BitBoard>& Board::color_occupancy() const { return m_color_occupancy; }
BitBoard Board::non_occupancy() const { return ~m_shared_occupancy; }

BitBoard Board::attacks(Color color) const
{
    if (m_color_attacks[color].none()) {
        auto mut_this = const_cast<Board*>(this);
        for (const auto& piece : Piece::all_valid_pieces) {
            if (piece.color != color)
                continue;

            auto occupancy = mut_this->m_piece_buffer.occupancy_for(piece);
            while (occupancy.any()) {
                auto origin = occupancy.pop_lsb().value();
                auto attacks = attack_maps::generate_attacks(piece, origin, m_shared_occupancy);
                mut_this->m_color_attacks[color] |= attacks;
            }
        }

        mut_this->m_color_attacks[color] &= ~m_color_occupancy[color];
    }

    return m_color_attacks[color];
}

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
    Board::Buffer piece_buffer {};

    for (auto i = 0; i < m_pieces.size(); i++) {
        Location location(i);
        Piece piece = m_pieces[i];
        piece_buffer.occupancy_for(piece).set(location);
    }

    return Board(piece_buffer);
}

}
