#include <array>

#include <weechess/fast/bit_board.h>
#include <weechess/fast/comptime.h>
#include <weechess/fast/move_generator.h>

namespace weechess::fast {

namespace {
    const std::array<BitBoard, 64> k_knight_attacks = comptime::compute_knight_attacks();
    const std::array<BitBoard, 64> k_king_attacks = comptime::compute_king_attacks();

    const comptime::RookMagicTable k_rook_magic_table = comptime::compute_rook_magic_table();
    const comptime::BishopMagicTable k_bishop_magic_table = comptime::compute_bishop_magic_table();

    const std::array<BitBoard, 64> k_rook_masks = comptime::compute_rook_slide_masks();
    const std::array<BitBoard, 64> k_bishop_masks = comptime::compute_bishop_slide_masks();

    class Analyzer {
    private:
        const MoveGenerator::Request& m_request;

    public:
        Analyzer(const MoveGenerator::Request& request)
            : m_request(request)
        {
        }

        BitBoard occupancy_for(const Color& color) const
        {
            BitBoard result {};
            for (const auto& type : Piece::types)
                result |= m_request.occupancy[Piece(type, color).representation];
            return result;
        }

        static BitBoard rook_attacks(Location location, BitBoard blockers)
        {
            blockers &= k_rook_masks[location.offset];
            auto blockers_ull = blockers.data();
            auto magic_ull = comptime::rook_magics[location.offset].data();
            auto key = (blockers_ull * magic_ull) >> (64 - comptime::rook_magic_indexes[location.offset]);
            return k_rook_magic_table[location.offset][key];
        }

        static BitBoard bishop_attacks(Location location, BitBoard blockers)
        {
            blockers &= k_bishop_masks[location.offset];
            auto blockers_ull = blockers.data();
            auto magic_ull = comptime::bishop_magics[location.offset].data();
            auto key = (blockers_ull * magic_ull) >> (64 - comptime::bishop_magic_indexes[location.offset]);
            return k_bishop_magic_table[location.offset][key];
        }
    };
}

MoveGenerator::Request::Request()
    : turn_to_move(Color::White)
    , castle_rights(CastleRights::all())
    , occupancy({})
    , en_passant_target({})
{
}

void MoveGenerator::Request::set_turn_to_move(Color color) { turn_to_move = color; }
void MoveGenerator::Request::set_castle_rights(Color color, CastleRights rights) { castle_rights[color] = rights; }
void MoveGenerator::Request::set_en_passant_target(const Location& location) { en_passant_target = location; }

void MoveGenerator::Request::add_piece(const Piece& piece, const Location& location)
{
    occupancy[piece.representation].set(location);
}

MoveGenerator::Result MoveGenerator::execute(const Request& request) const
{
    Analyzer a(request);
    Result result {};
    return result;
}

namespace internal {
    BitBoard rook_attacks(Location location, BitBoard blockers) { return Analyzer::rook_attacks(location, blockers); }
    BitBoard bishop_attacks(Location location, BitBoard blockers)
    {
        return Analyzer::bishop_attacks(location, blockers);
    }
}

} // namespace weechess
