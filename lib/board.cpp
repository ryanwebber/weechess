#include "board.h"

void test() {

}

namespace weechess {

    static constexpr std::string_view default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

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
    Board::Board(Piece cells[64]) {
        for (size_t i = 0; i < cell_count; i++) {
            this->cells[i] = cells[i];
        }
    }

    Piece Board::piece_at(Location location) const {
        return Piece(cells[location.offset]);
    }

    Piece Board::set_piece_at(Location location, Piece piece) {
        Piece old_piece = Piece(cells[location.offset]);
        cells[location.offset] = piece;
        return old_piece;
    }

    std::string Board::to_fen() const {
        std::string fen;
        uint8_t space_count = 0;
        for (size_t i = 0; i < cell_count; i++) {

            if (i % 8 == 0 && i != 0) {
                if (space_count > 0) {
                    fen += std::to_string(space_count);
                    space_count = 0;
                }

                fen += '/';
            }

            auto fen_char = Piece(cells[i]).to_fen();
            if (fen_char.has_value()) {
                if (space_count > 0) {
                    fen += std::to_string(space_count);
                    space_count = 0;
                }

                fen += fen_char.value();
            } else {
                space_count++;
            }
        }

        return fen;
    }

    std::optional<Board> Board::from_fen(std::string_view fen) {
        Piece cells[64];
        size_t i = 0;
        for (char c : fen) {
            std::optional<Piece> piece = Piece::from_fen(c);
            if (c == '/') {
                continue;
            } else if (std::isdigit(c)) {
                i += (c - '0');
            } else if (piece.has_value()) {
                cells[i] = piece.value();
                i++;
            } else {
                return std::nullopt;
            }
        }

        return Board(cells);
    }

    Board Board::default_board() {
        return Board::from_fen(default_fen).value();
    }
}
