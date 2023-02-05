#pragma once

#include <array>
#include <optional>
#include <span>
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
        Board(std::array<Piece, cell_count>);

        Piece piece_at(Location location) const;
        Piece set_piece_at(Location location, Piece piece);

        std::span<const Piece> cells() const;

    private:
        std::array<Piece, cell_count> m_cells;
    };
}
