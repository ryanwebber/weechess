#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "piece.h"

namespace weechess {

    struct Location {
        uint8_t offset;

        Location(uint8_t offset);

        uint8_t file();
        uint8_t rank();

        std::optional<Location> offset_by(int8_t offset);
    };

    class Board {
    public:
        static constexpr size_t cell_count = 64;

        Board();
        Board(Piece cells[64]);

        Piece piece_at(Location location) const;
        Piece set_piece_at(Location location, Piece piece);

        std::string to_fen() const;

        static std::optional<Board> from_fen(std::string_view);
        static Board default_board();

    private:
        Piece cells[cell_count];
    };

}
