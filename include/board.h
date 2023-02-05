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
        Location(uint8_t file, uint8_t rank);

        uint8_t file() const;
        uint8_t rank() const;

        std::string to_string() const;

        std::optional<Location> offset_by(int8_t offset);
    };

    class Board {
    public:
        static constexpr size_t cell_count = 64;
        using Buffer = std::array<Piece, cell_count>;

        Board();
        Board(Buffer);

        Piece piece_at(Location location) const;
        Piece set_piece_at(Location location, Piece piece);

        std::span<const Piece> cells() const;

    private:
        Buffer m_cells;
    };
}
