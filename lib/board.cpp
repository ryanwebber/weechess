#include "board.h"

namespace weechess {

    Location::Location(uint8_t offset)
        : offset(offset) {}

    uint8_t Location::file() {
        return offset % 8;
    }

    uint8_t Location::rank() {
        return offset / 8;
    }

    std::optional<Location> Location::offset_by(int8_t offset) {
        if (offset > 63) {
            return std::nullopt;
        }

        return Location(offset);
    }

    Board::Board() = default;
    Board::Board(std::array<Piece, cell_count> cells)
        : m_cells(cells) {}

    Piece Board::piece_at(Location location) const {
        return Piece(m_cells[location.offset]);
    }

    Piece Board::set_piece_at(Location location, Piece piece) {
        Piece old_piece = Piece(m_cells[location.offset]);
        m_cells[location.offset] = piece;
        return old_piece;
    }

    std::span<const Piece> Board::cells() const {
        return m_cells;
    }
}
