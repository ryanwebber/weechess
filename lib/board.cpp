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
        if (offset >= Board::cell_count) {
            return std::nullopt;
        }

        return Location(offset);
    }

    std::optional<Location> Location::offset_by(int8_t file_offset, int8_t rank_offset) {
        int8_t file = file_offset + this->file();
        int8_t rank = rank_offset + this->rank();

        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            return std::nullopt;
        }

        return Location(file, rank);
    }

    Location Location::from_rank_and_file(uint8_t rank, uint8_t file) {
        return Location(file, rank);
    }

    std::optional<Location> Location::from_name(std::string_view name) {
        if (name.size() != 2) {
            return std::nullopt;
        }

        uint8_t file = name[0] - 'a';
        uint8_t rank = name[1] - '1';

        if (file > 7 || rank > 7) {
            return std::nullopt;
        }

        return Location(file, rank);
    }

    Board::Board() = default;
    Board::Board(Buffer cells)
        : m_cells(cells) {}

    Piece Board::piece_at(Location location) const {
        return m_cells[location.offset];
    }

    Piece Board::set_piece_at(Location location, Piece piece) {
        Piece old_piece(m_cells[location.offset]);
        m_cells[location.offset] = piece;
        return old_piece;
    }

    Color Board::color_at(Location location) const {
        return (location.file() + location.rank()) % 2 == 0 ? Color::White : Color::Black;
    }

    std::span<const Piece> Board::cells() const {
        return m_cells;
    }
}
