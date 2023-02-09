#pragma once

#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <weechess/location.h>
#include <weechess/piece.h>

namespace weechess {

    class Board {
    public:
        static constexpr size_t cell_count = 64;
        using Buffer = std::array<Piece, cell_count>;

        Board();
        Board(Buffer);

        Piece piece_at(Location location) const;
        Piece set_piece_at(Location location, Piece piece);

        Color color_at(Location location) const;

        std::span<const Piece> cells() const;

    private:
        Buffer m_cells;
    };
}
