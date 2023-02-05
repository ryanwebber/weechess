#include "board.h"

namespace weechess {

    Location::Location(uint8_t offset)
        : offset(offset) {}

    Location::Location(uint8_t file, uint8_t rank)
        : offset(file + rank * 8) {}

    uint8_t Location::file() const {
        return offset % 8;
    }

    uint8_t Location::rank() const {
        return offset / 8;
    }

    std::string Location::to_string() const {
        return std::string(1, 'a' + file()) + std::to_string(rank() + 1);
    }

    std::optional<Location> Location::offset_by(int8_t offset) {
        if (offset > 63) {
            return std::nullopt;
        }

        return Location(offset);
    }

    Board::Board() = default;
    Board::Board(Buffer cells)
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
