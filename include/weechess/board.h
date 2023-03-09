#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <weechess/bit_board.h>
#include <weechess/color_map.h>
#include <weechess/location.h>
#include <weechess/piece.h>

namespace weechess {

namespace {
    constexpr std::array<BitBoard, 8> rank_masks = {
        BitBoard(0xffull),
        BitBoard(0xff00ull),
        BitBoard(0xff0000ull),
        BitBoard(0xff000000ull),
        BitBoard(0xff00000000ull),
        BitBoard(0xff0000000000ull),
        BitBoard(0xff000000000000ull),
        BitBoard(0xff00000000000000ull),
    };

    constexpr std::array<BitBoard, 8> file_masks = {
        BitBoard(0x8080808080808080ull),
        BitBoard(0x4040404040404040ull),
        BitBoard(0x2020202020202020ull),
        BitBoard(0x1010101010101010ull),
        BitBoard(0x0808080808080808ull),
        BitBoard(0x0404040404040404ull),
        BitBoard(0x0202020202020202ull),
        BitBoard(0x0101010101010101ull),
    };

}

struct Rank {
    uint8_t index;

    constexpr Rank(uint8_t named_rank)
        : index(named_rank - 1)
    {
    }

    constexpr Rank inverted() const { return Rank(8 - index); }

    constexpr BitBoard mask() const { return rank_masks[index]; }
};

struct File {
    uint8_t index;

    constexpr File(char named_file)
        : index(7 - (std::tolower(named_file) - 'a'))
    {
    }

    constexpr BitBoard mask() const { return file_masks[index]; }
};

constexpr bool operator==(const Rank& lhs, const Rank& rhs) { return lhs.index == rhs.index; }
constexpr bool operator!=(const Rank& lhs, const Rank& rhs) { return !(lhs == rhs); }
constexpr bool operator==(const File& lhs, const File& rhs) { return lhs.index == rhs.index; }
constexpr bool operator!=(const File& lhs, const File& rhs) { return !(lhs == rhs); }

inline std::ostream& operator<<(std::ostream& stream, const Rank& rank)
{
    return stream << "Rank(" << static_cast<int>(rank.index + 1) << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const File& file)
{
    return stream << "File(" << static_cast<char>('A' + (7 - file.index)) << ")";
}

class Board {
public:
    static constexpr size_t cell_count = 64;

    struct Buffer {
        std::array<BitBoard, 14> m_occupancy;

        Buffer() = default;

        const std::array<BitBoard, 14>& data() const { return m_occupancy; }

        BitBoard& occupancy_for(Piece piece);
        const BitBoard& occupancy_for(Piece piece) const;
    };

    Board();
    Board(Buffer);

    Piece piece_at(Location location) const;
    Color color_at(Location location) const;

    const Buffer& piece_buffer() const;
    const BitBoard& occupancy_for(Piece piece) const;
    const BitBoard& shared_occupancy() const;
    const ColorMap<BitBoard>& color_occupancy() const;
    BitBoard attacks(Color color) const;
    BitBoard pawn_attacks(Color color) const;
    BitBoard non_occupancy() const;

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
    Buffer m_piece_buffer {};
    BitBoard m_shared_occupancy {};
    ColorMap<BitBoard> m_color_occupancy {};
    ColorMap<BitBoard> m_color_attacks {};
    ColorMap<BitBoard> m_pawn_attacks {};
};

}
