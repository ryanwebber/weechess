#include <weechess/attack_maps.h>
#include <weechess/board.h>
#include <weechess/color_map.h>

namespace weechess {

namespace {

    constexpr std::array<BitBoard, 64> rook_magics = {
        BitBoard(0xa8002c000108020ULL),
        BitBoard(0x6c00049b0002001ULL),
        BitBoard(0x100200010090040ULL),
        BitBoard(0x2480041000800801ULL),
        BitBoard(0x280028004000800ULL),
        BitBoard(0x900410008040022ULL),
        BitBoard(0x280020001001080ULL),
        BitBoard(0x2880002041000080ULL),
        BitBoard(0xa000800080400034ULL),
        BitBoard(0x4808020004000ULL),
        BitBoard(0x2290802004801000ULL),
        BitBoard(0x411000d00100020ULL),
        BitBoard(0x402800800040080ULL),
        BitBoard(0xb000401004208ULL),
        BitBoard(0x2409000100040200ULL),
        BitBoard(0x1002100004082ULL),
        BitBoard(0x22878001e24000ULL),
        BitBoard(0x1090810021004010ULL),
        BitBoard(0x801030040200012ULL),
        BitBoard(0x500808008001000ULL),
        BitBoard(0xa08018014000880ULL),
        BitBoard(0x8000808004000200ULL),
        BitBoard(0x201008080010200ULL),
        BitBoard(0x801020000441091ULL),
        BitBoard(0x800080204005ULL),
        BitBoard(0x1040200040100048ULL),
        BitBoard(0x120200402082ULL),
        BitBoard(0xd14880480100080ULL),
        BitBoard(0x12040280080080ULL),
        BitBoard(0x100040080020080ULL),
        BitBoard(0x9020010080800200ULL),
        BitBoard(0x813241200148449ULL),
        BitBoard(0x491604001800080ULL),
        BitBoard(0x100401000402001ULL),
        BitBoard(0x4820010021001040ULL),
        BitBoard(0x400402202000812ULL),
        BitBoard(0x209009005000802ULL),
        BitBoard(0x810800601800400ULL),
        BitBoard(0x4301083214000150ULL),
        BitBoard(0x204026458e001401ULL),
        BitBoard(0x40204000808000ULL),
        BitBoard(0x8001008040010020ULL),
        BitBoard(0x8410820820420010ULL),
        BitBoard(0x1003001000090020ULL),
        BitBoard(0x804040008008080ULL),
        BitBoard(0x12000810020004ULL),
        BitBoard(0x1000100200040208ULL),
        BitBoard(0x430000a044020001ULL),
        BitBoard(0x280009023410300ULL),
        BitBoard(0xe0100040002240ULL),
        BitBoard(0x200100401700ULL),
        BitBoard(0x2244100408008080ULL),
        BitBoard(0x8000400801980ULL),
        BitBoard(0x2000810040200ULL),
        BitBoard(0x8010100228810400ULL),
        BitBoard(0x2000009044210200ULL),
        BitBoard(0x4080008040102101ULL),
        BitBoard(0x40002080411d01ULL),
        BitBoard(0x2005524060000901ULL),
        BitBoard(0x502001008400422ULL),
        BitBoard(0x489a000810200402ULL),
        BitBoard(0x1004400080a13ULL),
        BitBoard(0x4000011008020084ULL),
        BitBoard(0x26002114058042ULL),
    };

    const std::array<BitBoard, 64> bishop_magics = {
        BitBoard(0x89a1121896040240ULL),
        BitBoard(0x2004844802002010ULL),
        BitBoard(0x2068080051921000ULL),
        BitBoard(0x62880a0220200808ULL),
        BitBoard(0x0004042004000000ULL),
        BitBoard(0x0100822020200011ULL),
        BitBoard(0xc00444222012000aULL),
        BitBoard(0x0028808801216001ULL),
        BitBoard(0x0400492088408100ULL),
        BitBoard(0x0201c401040c0084ULL),
        BitBoard(0x00840800910a0010ULL),
        BitBoard(0x0000082080240060ULL),
        BitBoard(0x2000840504006000ULL),
        BitBoard(0x30010c4108405004ULL),
        BitBoard(0x1008005410080802ULL),
        BitBoard(0x8144042209100900ULL),
        BitBoard(0x0208081020014400ULL),
        BitBoard(0x004800201208ca00ULL),
        BitBoard(0x0f18140408012008ULL),
        BitBoard(0x1004002802102001ULL),
        BitBoard(0x0841000820080811ULL),
        BitBoard(0x0040200200a42008ULL),
        BitBoard(0x0000800054042000ULL),
        BitBoard(0x88010400410c9000ULL),
        BitBoard(0x0520040470104290ULL),
        BitBoard(0x1004040051500081ULL),
        BitBoard(0x2002081833080021ULL),
        BitBoard(0x000400c00c010142ULL),
        BitBoard(0x941408200c002000ULL),
        BitBoard(0x0658810000806011ULL),
        BitBoard(0x0188071040440a00ULL),
        BitBoard(0x4800404002011c00ULL),
        BitBoard(0x0104442040404200ULL),
        BitBoard(0x0511080202091021ULL),
        BitBoard(0x0004022401120400ULL),
        BitBoard(0x80c0040400080120ULL),
        BitBoard(0x8040010040820802ULL),
        BitBoard(0x0480810700020090ULL),
        BitBoard(0x0102008e00040242ULL),
        BitBoard(0x0809005202050100ULL),
        BitBoard(0x8002024220104080ULL),
        BitBoard(0x0431008804142000ULL),
        BitBoard(0x0019001802081400ULL),
        BitBoard(0x0200014208040080ULL),
        BitBoard(0x3308082008200100ULL),
        BitBoard(0x041010500040c020ULL),
        BitBoard(0x4012020c04210308ULL),
        BitBoard(0x208220a202004080ULL),
        BitBoard(0x0111040120082000ULL),
        BitBoard(0x6803040141280a00ULL),
        BitBoard(0x2101004202410000ULL),
        BitBoard(0x8200000041108022ULL),
        BitBoard(0x0000021082088000ULL),
        BitBoard(0x0002410204010040ULL),
        BitBoard(0x0040100400809000ULL),
        BitBoard(0x0822088220820214ULL),
        BitBoard(0x0040808090012004ULL),
        BitBoard(0x00910224040218c9ULL),
        BitBoard(0x0402814422015008ULL),
        BitBoard(0x0090014004842410ULL),
        BitBoard(0x0001000042304105ULL),
        BitBoard(0x0010008830412a00ULL),
        BitBoard(0x2520081090008908ULL),
        BitBoard(0x40102000a0a60140ULL),
    };

    const std::array<int, 64> rook_magic_indexes = {
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

    const std::array<int, 64> bishop_magic_indexes = {
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

    enum Direction : u_int8_t {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,
    };

    constexpr std::array<Direction, 8> directions = {
        Direction::North,
        Direction::NorthEast,
        Direction::East,
        Direction::SouthEast,
        Direction::South,
        Direction::SouthWest,
        Direction::West,
        Direction::NorthWest,
    };

    constexpr std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> directionSteps = {
        std::make_tuple(Location::FileShift {}, Location::Up),
        std::make_tuple(Location::Right, Location::Up),
        std::make_tuple(Location::Right, Location::RankShift {}),
        std::make_tuple(Location::Right, Location::Down),
        std::make_tuple(Location::FileShift {}, Location::Down),
        std::make_tuple(Location::Left, Location::Down),
        std::make_tuple(Location::Left, Location::RankShift {}),
        std::make_tuple(Location::Left, Location::Up),
    };

    constexpr BitBoard compute_ray(Location location, Location::FileShift fs, Location::RankShift rs)
    {
        BitBoard bb;
        std::optional<Location> lprime = location;
        while ((lprime = lprime->offset_by(fs, rs))) {
            bb.set(*lprime);
        }

        return bb;
    }

    constexpr std::array<std::array<BitBoard, 8>, 64> compute_all_rays()
    {
        std::array<std::array<BitBoard, 8>, 64> rays { {} };
        for (auto i = 0; i < 64; i++) {
            Location location(i);
            for (const auto& direction : directions) {
                const auto& steps = directionSteps[direction];
                rays[i][direction] = compute_ray(location, std::get<0>(steps), std::get<1>(steps));
            }
        }

        return rays;
    }

    constexpr std::array<std::array<BitBoard, 8>, 64> rays = compute_all_rays();

    constexpr BitBoard compute_blockers_from_index(unsigned int index, BitBoard mask)
    {
        BitBoard bb;
        int bits = mask.count_set_bits();
        for (int i = 0; i < bits; i++) {
            auto next_bit = mask.pop_lsb().value();
            if ((index & (1 << i)) != 0) {
                BitBoard bb_next;
                bb_next.set(next_bit);
                bb |= bb_next;
            }
        }

        return bb;
    }

    constexpr std::array<BitBoard, 64> compute_rook_slide_masks()
    {
        std::array<BitBoard, 64> masks {};
        for (auto i = 0; i < 64; i++) {
            Location l(i);

            masks[i] |= rays[i][Direction::West] & ~File('A').mask();
            masks[i] |= rays[i][Direction::East] & ~File('H').mask();
            masks[i] |= rays[i][Direction::North] & ~Rank(8).mask();
            masks[i] |= rays[i][Direction::South] & ~Rank(1).mask();
        }

        return masks;
    }

    constexpr std::array<BitBoard, 64> compute_bishop_slide_masks()
    {
        std::array<BitBoard, 64> masks {};
        for (auto i = 0; i < 64; i++) {
            Location l(i);

            masks[i] |= rays[i][Direction::NorthWest] & ~(File('A').mask() | Rank(8).mask());
            masks[i] |= rays[i][Direction::SouthWest] & ~(File('A').mask() | Rank(1).mask());
            masks[i] |= rays[i][Direction::NorthEast] & ~(File('H').mask() | Rank(8).mask());
            masks[i] |= rays[i][Direction::SouthEast] & ~(File('H').mask() | Rank(1).mask());
        }

        return masks;
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

    constexpr BitBoard compute_rook_attacks_unoptimized(Location location, BitBoard blockers)
    {
        BitBoard bb;

        auto up_ray = rays[location.offset][Direction::North];
        auto down_ray = rays[location.offset][Direction::South];
        auto right_ray = rays[location.offset][Direction::East];
        auto left_ray = rays[location.offset][Direction::West];

        // Up
        bb |= up_ray;
        if ((up_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((up_ray & blockers).lsb().value()).offset][Direction::North]);
        }

        // Down
        bb |= down_ray;
        if ((down_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((down_ray & blockers).msb().value()).offset][Direction::South]);
        }

        // Right
        bb |= right_ray;
        if ((right_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((right_ray & blockers).lsb().value()).offset][Direction::East]);
        }

        // Left
        bb |= left_ray;
        if ((left_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((left_ray & blockers).msb().value()).offset][Direction::West]);
        }

        return bb;
    }

    constexpr RookMagicTable compute_rook_magic_table()
    {
        RookMagicTable table { {} };
        auto rook_mask_table = compute_rook_slide_masks();
        for (auto i = 0; i < 64; i++) {
            Location l(i);
            for (unsigned int b = 0; b < (1 << rook_magic_indexes[i]); b++) {

                BitBoard blockers = compute_blockers_from_index(b, rook_mask_table[i]);

                auto blocker_ull = blockers.data();
                auto magic_ull = rook_magics[i].data();
                auto table_index = (blocker_ull * magic_ull) >> (64 - rook_magic_indexes[i]);

                assert(table_index < table[i].size());

                table[i][table_index] = compute_rook_attacks_unoptimized(l, blockers);
            }
        }

        return table;
    }

    constexpr BitBoard compute_bishop_attacks_unoptimized(Location location, BitBoard blockers)
    {
        BitBoard bb;

        auto up_right_ray = rays[location.offset][Direction::NorthEast];
        auto down_right_ray = rays[location.offset][Direction::SouthEast];
        auto down_left_ray = rays[location.offset][Direction::SouthWest];
        auto up_left_ray = rays[location.offset][Direction::NorthWest];

        // Up Right
        bb |= up_right_ray;
        if ((up_right_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((up_right_ray & blockers).lsb().value()).offset][Direction::NorthEast]);
        }

        // Down Right
        bb |= down_right_ray;
        if ((down_right_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((down_right_ray & blockers).msb().value()).offset][Direction::SouthEast]);
        }

        // Down Left
        bb |= down_left_ray;
        if ((down_left_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((down_left_ray & blockers).msb().value()).offset][Direction::SouthWest]);
        }

        // Up Left
        bb |= up_left_ray;
        if ((up_left_ray & blockers) != BitBoard::empty()) {
            bb &= ~(rays[Location((up_left_ray & blockers).lsb().value()).offset][Direction::NorthWest]);
        }

        return bb;
    }

    constexpr BishopMagicTable compute_bishop_magic_table()
    {
        BishopMagicTable table { {} };
        auto bishop_mask_table = compute_bishop_slide_masks();
        for (auto i = 0; i < 64; i++) {
            Location l(i);
            for (unsigned int b = 0; b < (1 << bishop_magic_indexes[i]); b++) {

                BitBoard blockers = compute_blockers_from_index(b, bishop_mask_table[i]);

                auto blocker_ull = blockers.data();
                auto magic_ull = bishop_magics[i].data();
                auto table_index = (blocker_ull * magic_ull) >> (64 - bishop_magic_indexes[i]);

                assert(table_index < table[i].size());

                table[i][table_index] = compute_bishop_attacks_unoptimized(l, blockers);
            }
        }

        return table;
    }
}

const RookMagicTable k_rook_magic_table = compute_rook_magic_table();
const BishopMagicTable k_bishop_magic_table = compute_bishop_magic_table();

const std::array<BitBoard, 64> k_knight_attacks = compute_knight_attacks();
const std::array<BitBoard, 64> k_king_attacks = compute_king_attacks();
const ColorMap<std::array<BitBoard, 64>> k_pawn_attacks = compute_pawn_attacks();

const std::array<BitBoard, 64> k_rook_masks = compute_rook_slide_masks();
const std::array<BitBoard, 64> k_bishop_masks = compute_bishop_slide_masks();

namespace attack_maps {

    BitBoard generate_knight_attacks(Location location) { return k_knight_attacks[location.offset]; }

    BitBoard generate_king_attacks(Location location) { return k_king_attacks[location.offset]; }

    BitBoard generate_pawn_attacks(Location location, Color color) { return k_pawn_attacks[color][location.offset]; }

    BitBoard generate_rook_attacks(Location location, BitBoard blockers)
    {
        blockers &= k_rook_masks[location.offset];
        auto blockers_ull = blockers.data();
        auto magic_ull = rook_magics[location.offset].data();
        auto key = (blockers_ull * magic_ull) >> (64 - rook_magic_indexes[location.offset]);
        return k_rook_magic_table[location.offset][key];
    }

    BitBoard generate_bishop_attacks(Location location, BitBoard blockers)
    {
        blockers &= k_bishop_masks[location.offset];
        auto blockers_ull = blockers.data();
        auto magic_ull = bishop_magics[location.offset].data();
        auto key = (blockers_ull * magic_ull) >> (64 - bishop_magic_indexes[location.offset]);
        return k_bishop_magic_table[location.offset][key];
    }

    BitBoard generate_queen_attacks(Location location, BitBoard blockers)
    {
        return generate_rook_attacks(location, blockers) | generate_bishop_attacks(location, blockers);
    }
}
}
