#pragma once

#include <array>
#include <optional>
#include <weechess/color_map.h>
#include <weechess/fast/bit_board.h>
#include <weechess/piece.h>

namespace weechess::fast::comptime {

constexpr uint8_t A_File = 7;
constexpr uint8_t H_File = 0;
constexpr uint8_t _1st_Rank = 0;
constexpr uint8_t _8th_Rank = 7;

constexpr std::array<BitBoard, 8> rank_mask = {
    // clang-format off
    BitBoard(0xffull),
    BitBoard(0xff00ull),
    BitBoard(0xff0000ull),
    BitBoard(0xff000000ull),
    BitBoard(0xff00000000ull),
    BitBoard(0xff0000000000ull),
    BitBoard(0xff000000000000ull),
    BitBoard(0xff00000000000000ull),
    // clang-format on
};

constexpr std::array<BitBoard, 8> file_mask = {
    // clang-format off
    BitBoard(0x8080808080808080ull),
    BitBoard(0x4040404040404040ull),
    BitBoard(0x2020202020202020ull),
    BitBoard(0x1010101010101010ull),
    BitBoard(0x808080808080808ull),
    BitBoard(0x404040404040404ull),
    BitBoard(0x202020202020202ull),
    BitBoard(0x101010101010101ull),
    // clang-format on
};

constexpr ColorMap<BitBoard> square_colors(
    // clang-format off
    BitBoard(0x55AA55AA55AA55AAULL), // white
    BitBoard(0xAA55AA55AA55AA55ULL) // black
    // clang-format on
);

constexpr std::array<Color, 2> colors = {
    Color::White,
    Color::Black,
};

constexpr std::array<BitBoard, 64> rook_magics = {
    // clang-format off
    BitBoard(0x0a8002c000108020ULL), BitBoard(0x06c00049b0002001ULL),
    BitBoard(0x0100200010090040ULL), BitBoard(0x2480041000800801ULL),
    BitBoard(0x0280028004000800ULL), BitBoard(0x0900410008040022ULL),
    BitBoard(0x0280020001001080ULL), BitBoard(0x2880002041000080ULL),
    BitBoard(0xa000800080400034ULL), BitBoard(0x0004808020004000ULL),
    BitBoard(0x2290802004801000ULL), BitBoard(0x0411000d00100020ULL),
    BitBoard(0x0402800800040080ULL), BitBoard(0x000b000401004208ULL),
    BitBoard(0x2409000100040200ULL), BitBoard(0x0001002100004082ULL),
    BitBoard(0x0022878001e24000ULL), BitBoard(0x1090810021004010ULL),
    BitBoard(0x0801030040200012ULL), BitBoard(0x0500808008001000ULL),
    BitBoard(0x0a08018014000880ULL), BitBoard(0x8000808004000200ULL),
    BitBoard(0x0201008080010200ULL), BitBoard(0x0801020000441091ULL),
    BitBoard(0x0000800080204005ULL), BitBoard(0x1040200040100048ULL),
    BitBoard(0x0000120200402082ULL), BitBoard(0x0d14880480100080ULL),
    BitBoard(0x0012040280080080ULL), BitBoard(0x0100040080020080ULL),
    BitBoard(0x9020010080800200ULL), BitBoard(0x0813241200148449ULL),
    BitBoard(0x0491604001800080ULL), BitBoard(0x0100401000402001ULL),
    BitBoard(0x4820010021001040ULL), BitBoard(0x0400402202000812ULL),
    BitBoard(0x0209009005000802ULL), BitBoard(0x0810800601800400ULL),
    BitBoard(0x4301083214000150ULL), BitBoard(0x204026458e001401ULL),
    BitBoard(0x0040204000808000ULL), BitBoard(0x8001008040010020ULL),
    BitBoard(0x8410820820420010ULL), BitBoard(0x1003001000090020ULL),
    BitBoard(0x0804040008008080ULL), BitBoard(0x0012000810020004ULL),
    BitBoard(0x1000100200040208ULL), BitBoard(0x430000a044020001ULL),
    BitBoard(0x0280009023410300ULL), BitBoard(0x00e0100040002240ULL),
    BitBoard(0x0000200100401700ULL), BitBoard(0x2244100408008080ULL),
    BitBoard(0x0008000400801980ULL), BitBoard(0x0002000810040200ULL),
    BitBoard(0x8010100228810400ULL), BitBoard(0x2000009044210200ULL),
    BitBoard(0x4080008040102101ULL), BitBoard(0x0040002080411d01ULL),
    BitBoard(0x2005524060000901ULL), BitBoard(0x0502001008400422ULL),
    BitBoard(0x489a000810200402ULL), BitBoard(0x0001004400080a13ULL),
    BitBoard(0x4000011008020084ULL), BitBoard(0x0026002114058042ULL),
    // clang-format on
};

const std::array<BitBoard, 64> bishop_magics = {
    // clang-format off
    BitBoard(0x89a1121896040240ULL), BitBoard(0x2004844802002010ULL),
    BitBoard(0x2068080051921000ULL), BitBoard(0x62880a0220200808ULL),
    BitBoard(0x0004042004000000ULL), BitBoard(0x0100822020200011ULL),
    BitBoard(0xc00444222012000aULL), BitBoard(0x0028808801216001ULL),
    BitBoard(0x0400492088408100ULL), BitBoard(0x0201c401040c0084ULL),
    BitBoard(0x00840800910a0010ULL), BitBoard(0x0000082080240060ULL),
    BitBoard(0x2000840504006000ULL), BitBoard(0x30010c4108405004ULL),
    BitBoard(0x1008005410080802ULL), BitBoard(0x8144042209100900ULL),
    BitBoard(0x0208081020014400ULL), BitBoard(0x004800201208ca00ULL),
    BitBoard(0x0f18140408012008ULL), BitBoard(0x1004002802102001ULL),
    BitBoard(0x0841000820080811ULL), BitBoard(0x0040200200a42008ULL),
    BitBoard(0x0000800054042000ULL), BitBoard(0x88010400410c9000ULL),
    BitBoard(0x0520040470104290ULL), BitBoard(0x1004040051500081ULL),
    BitBoard(0x2002081833080021ULL), BitBoard(0x000400c00c010142ULL),
    BitBoard(0x941408200c002000ULL), BitBoard(0x0658810000806011ULL),
    BitBoard(0x0188071040440a00ULL), BitBoard(0x4800404002011c00ULL),
    BitBoard(0x0104442040404200ULL), BitBoard(0x0511080202091021ULL),
    BitBoard(0x0004022401120400ULL), BitBoard(0x80c0040400080120ULL),
    BitBoard(0x8040010040820802ULL), BitBoard(0x0480810700020090ULL),
    BitBoard(0x0102008e00040242ULL), BitBoard(0x0809005202050100ULL),
    BitBoard(0x8002024220104080ULL), BitBoard(0x0431008804142000ULL),
    BitBoard(0x0019001802081400ULL), BitBoard(0x0200014208040080ULL),
    BitBoard(0x3308082008200100ULL), BitBoard(0x041010500040c020ULL),
    BitBoard(0x4012020c04210308ULL), BitBoard(0x208220a202004080ULL),
    BitBoard(0x0111040120082000ULL), BitBoard(0x6803040141280a00ULL),
    BitBoard(0x2101004202410000ULL), BitBoard(0x8200000041108022ULL),
    BitBoard(0x0000021082088000ULL), BitBoard(0x0002410204010040ULL),
    BitBoard(0x0040100400809000ULL), BitBoard(0x0822088220820214ULL),
    BitBoard(0x0040808090012004ULL), BitBoard(0x00910224040218c9ULL),
    BitBoard(0x0402814422015008ULL), BitBoard(0x0090014004842410ULL),
    BitBoard(0x0001000042304105ULL), BitBoard(0x0010008830412a00ULL),
    BitBoard(0x2520081090008908ULL), BitBoard(0x40102000a0a60140ULL),
    // clang-format on
};

const std::array<uint8_t, 64> rook_magic_indexes = {
    // clang-format off
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12,
    // clang-format on
};

const std::array<uint8_t, 64> bishop_magic_indexes = {
    // clang-format off
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6,
    // clang-format on
};

constexpr size_t rook_magic_table_size = 4096;
constexpr size_t bishop_magic_table_size = 1024;

using RookMagicTable = std::array<std::array<BitBoard, rook_magic_table_size>, 64>;
using BishopMagicTable = std::array<std::array<BitBoard, bishop_magic_table_size>, 64>;

constexpr BitBoard compute_ray(Location location, Location::FileShift fs, Location::RankShift rs)
{
    BitBoard bb;
    std::optional<Location> lprime = location;
    while ((lprime = lprime->offset_by(fs, rs))) {
        bb.set(*lprime);
    }

    return bb;
}

constexpr ColorMap<std::array<BitBoard, 64>> compute_pawn_attacks()
{
    ColorMap<std::array<BitBoard, 64>> attacks;
    for (auto i = 0; i < 64; i++) {
        Location l(i);

        if (auto lwl = l.offset_by(Location::Left, Location::Up))
            attacks[Color::White][l.offset].set(*lwl);
        if (auto lwr = l.offset_by(Location::Right, Location::Up))
            attacks[Color::White][l.offset].set(*lwr);

        if (auto lbl = l.offset_by(Location::Left, Location::Down))
            attacks[Color::Black][l.offset].set(*lbl);
        if (auto lbr = l.offset_by(Location::Right, Location::Down))
            attacks[Color::Black][l.offset].set(*lbr);
    }

    return attacks;
}

constexpr std::array<BitBoard, 64> compute_knight_attacks()
{
    constexpr std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> knight_jumps = {
        std::make_tuple(Location::Left * 2, Location::Down),
        std::make_tuple(Location::Left * 2, Location::Up),
        std::make_tuple(Location::Right * 2, Location::Down),
        std::make_tuple(Location::Right * 2, Location::Up),
        std::make_tuple(Location::Left, Location::Down * 2),
        std::make_tuple(Location::Left, Location::Up * 2),
        std::make_tuple(Location::Right, Location::Down * 2),
        std::make_tuple(Location::Right, Location::Up * 2),
    };

    std::array<BitBoard, 64> results {};
    for (auto i = 0; i < results.size(); i++) {
        Location location(i);
        for (const auto& jump : knight_jumps) {
            auto [file_shift, rank_shift] = jump;
            if (auto new_location = location.offset_by(file_shift, rank_shift)) {
                results[i].set(*new_location);
            }
        }
    }

    return results;
}

constexpr std::array<BitBoard, 64> compute_king_attacks()
{
    constexpr std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> king_jumps = {
        std::make_tuple(Location::Left, Location::Down),
        std::make_tuple(Location::Left, Location::Up),
        std::make_tuple(Location::Right, Location::Down),
        std::make_tuple(Location::Right, Location::Up),
        std::make_tuple(Location::Left, Location::RankShift {}),
        std::make_tuple(Location::Right, Location::RankShift {}),
        std::make_tuple(Location::FileShift {}, Location::Down),
        std::make_tuple(Location::FileShift {}, Location::Up),
    };

    std::array<BitBoard, 64> results {};
    for (auto i = 0; i < results.size(); i++) {
        Location location(i);
        for (const auto& jump : king_jumps) {
            auto [file_shift, rank_shift] = jump;
            if (auto new_location = location.offset_by(file_shift, rank_shift)) {
                results[i].set(*new_location);
            }
        }
    }

    return results;
}

constexpr std::array<BitBoard, 64> compute_rook_slide_masks()
{
    std::array<BitBoard, 64> masks {};
    for (auto i = 0; i < 64; i++) {
        Location l(i);

        masks[i] |= (compute_ray(l, Location::Left, {}) & ~file_mask[A_File]);
        masks[i] |= (compute_ray(l, Location::Right, {}) & ~file_mask[H_File]);
        masks[i] |= (compute_ray(l, {}, Location::Up) & ~rank_mask[_8th_Rank]);
        masks[i] |= (compute_ray(l, {}, Location::Down) & ~rank_mask[_1st_Rank]);
    }

    return masks;
}

constexpr std::array<BitBoard, 64> compute_bishop_slide_masks()
{
    std::array<BitBoard, 64> masks {};
    for (auto i = 0; i < 64; i++) {
        Location l(i);

        masks[i] |= (compute_ray(l, Location::Left, Location::Up) & ~(file_mask[A_File] | rank_mask[_8th_Rank]));
        masks[i] |= (compute_ray(l, Location::Left, Location::Down) & ~(file_mask[H_File] | rank_mask[_1st_Rank]));
        masks[i] |= (compute_ray(l, Location::Right, Location::Up) & ~(rank_mask[_8th_Rank] | rank_mask[_8th_Rank]));
        masks[i] |= (compute_ray(l, Location::Right, Location::Down) & ~(rank_mask[_1st_Rank] | rank_mask[_1st_Rank]));
    }

    return masks;
}

constexpr BitBoard compute_blockers_from_index(uint8_t index, BitBoard mask)
{
    BitBoard bb;
    int bits = mask.count_set_bits();
    for (int i = 0; i < bits; i++) {
        auto next_bit = mask.pop_lsb().value();
        if (index & (1 << i)) {
            BitBoard bb_next;
            bb_next.set(next_bit);
            bb |= bb_next;
        }
    }

    return bb;
}

constexpr BitBoard compute_rook_attacks_unoptimized(Location location, BitBoard blockers)
{
    BitBoard bb;

    auto up_ray = compute_ray(location, {}, Location::Up);
    auto down_ray = compute_ray(location, {}, Location::Down);
    auto right_ray = compute_ray(location, Location::Right, {});
    auto left_ray = compute_ray(location, Location::Left, {});

    // Up
    bb |= up_ray;
    if ((up_ray & blockers) != BitBoard::empty()) {
        bb &= ~(compute_ray(Location((up_ray & blockers).lsb().value()), {}, Location::Up));
    }

    // Down
    bb |= down_ray;
    if ((down_ray & blockers) != BitBoard::empty()) {
        bb &= ~(compute_ray(Location((down_ray & blockers).msb().value()), {}, Location::Down));
    }

    // Right
    bb |= right_ray;
    if ((right_ray & blockers) != BitBoard::empty()) {
        bb &= ~(compute_ray(Location((right_ray & blockers).lsb().value()), Location::Right, {}));
    }

    // Left
    bb |= left_ray;
    if ((left_ray & blockers) != BitBoard::empty()) {
        bb &= ~(compute_ray(Location((left_ray & blockers).msb().value()), Location::Left, {}));
    }

    return bb;
}

constexpr RookMagicTable compute_rook_magic_table()
{
    RookMagicTable table {};
    auto rook_mask_table = compute_rook_slide_masks();
    for (auto i = 0; i < 64; i++) {
        Location l(i);
        for (int b = 0; b < (1 << rook_magic_indexes[i]); b++) {

            BitBoard blockers = compute_blockers_from_index(b, rook_mask_table[i]);

            auto blocker_ull = blockers.data().to_ullong();
            auto magic_ull = rook_magics[i].data().to_ullong();
            auto table_index = (blocker_ull * magic_ull) >> (64 - rook_magic_indexes[i]);
            table[i][table_index] = compute_rook_attacks_unoptimized(l, blockers);
        }
    }

    return table;
}

}
