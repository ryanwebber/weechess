#pragma once

#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <weechess/bit_board.h>
#include <weechess/location.h>
#include <weechess/piece.h>

namespace weechess {

class Board {
public:
    static constexpr size_t cell_count = 64;

    using Buffer = std::array<BitBoard, 12>;

    Board();
    Board(Buffer);

    Piece piece_at(Location location) const;
    Color color_at(Location location) const;

    BitBoard& occupancy_for(Piece piece);
    const BitBoard& occupancy_for(Piece piece) const;

    std::array<Piece, 64> to_array() const;

    class Builder {
    public:
        Builder() = default;
        Builder& set_piece(const Piece&, const Location&);
        Board build() const;

        Piece& operator[](const Location& location);
        const Piece& operator[](const Location& location) const;

    private:
        std::array<Piece, 64> m_pieces {};
    };

private:
    Buffer m_occupancy;
};

}
