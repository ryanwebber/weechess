#include <array>

#include <weechess/bit_board.h>
#include <weechess/comptime.h>
#include <weechess/move_generator.h>

namespace weechess {

namespace {
    const std::array<BitBoard, 64> k_knight_attacks = comptime::compute_knight_attacks();
    const std::array<BitBoard, 64> k_king_attacks = comptime::compute_king_attacks();

    const comptime::RookMagicTable k_rook_magic_table = comptime::compute_rook_magic_table();
    const comptime::BishopMagicTable k_bishop_magic_table = comptime::compute_bishop_magic_table();

    const std::array<BitBoard, 64> k_rook_masks = comptime::compute_rook_slide_masks();
    const std::array<BitBoard, 64> k_bishop_masks = comptime::compute_bishop_slide_masks();

    class Helper {
    private:
        const MoveGenerator::Request& m_request;

        BitBoard m_shared_occupancy;

    public:
        Helper(const MoveGenerator::Request& request)
            : m_request(request)
            , m_shared_occupancy({})
        {
            m_shared_occupancy |= occupancy(Piece::Type::Pawn, Color::White);
            m_shared_occupancy |= occupancy(Piece::Type::Pawn, Color::Black);
            m_shared_occupancy |= occupancy(Piece::Type::Knight, Color::White);
            m_shared_occupancy |= occupancy(Piece::Type::Knight, Color::Black);
            m_shared_occupancy |= occupancy(Piece::Type::Bishop, Color::White);
            m_shared_occupancy |= occupancy(Piece::Type::Bishop, Color::Black);
            m_shared_occupancy |= occupancy(Piece::Type::Rook, Color::White);
            m_shared_occupancy |= occupancy(Piece::Type::Rook, Color::Black);
            m_shared_occupancy |= occupancy(Piece::Type::Queen, Color::White);
            m_shared_occupancy |= occupancy(Piece::Type::Queen, Color::Black);
            m_shared_occupancy |= occupancy(Piece::Type::King, Color::White);
            m_shared_occupancy |= occupancy(Piece::Type::King, Color::Black);
        }

        Piece piece_to_move(Piece::Type type) const { return Piece(type, m_request.turn_to_move); }

        Location forward(Location location) const
        {
            if (m_request.turn_to_move == Color::White)
                return location.offset_by(Location::Up).value();
            else
                return location.offset_by(Location::Down).value();
        }

        BitBoard backrank_mask() const
        {
            if (m_request.turn_to_move == Color::White)
                return comptime::rank_mask[comptime::_8th_Rank];
            else
                return comptime::rank_mask[comptime::_1st_Rank];
        }

        BitBoard shift_forward(BitBoard bb) const
        {
            if (m_request.turn_to_move == Color::White)
                return BitBoard(bb.data() << 8);
            else
                return BitBoard(bb.data() >> 8);
        }

        BitBoard occupancy() const { return m_shared_occupancy; }

        BitBoard occupancy(Piece::Type type, Color color) const
        {
            return m_request.occupancy[Piece(type, color).representation];
        }

        BitBoard occupancy(Piece::Type type) const { return occupancy(type, m_request.turn_to_move); }

        BitBoard non_occupancy() const { return ~m_shared_occupancy; }
    };

    BitBoard generate_rook_attacks(Location location, BitBoard blockers)
    {
        blockers &= k_rook_masks[location.offset];
        auto blockers_ull = blockers.data();
        auto magic_ull = comptime::rook_magics[location.offset].data();
        auto key = (blockers_ull * magic_ull) >> (64 - comptime::rook_magic_indexes[location.offset]);
        return k_rook_magic_table[location.offset][key];
    }

    BitBoard generate_bishop_attacks(Location location, BitBoard blockers)
    {
        blockers &= k_bishop_masks[location.offset];
        auto blockers_ull = blockers.data();
        auto magic_ull = comptime::bishop_magics[location.offset].data();
        auto key = (blockers_ull * magic_ull) >> (64 - comptime::bishop_magic_indexes[location.offset]);
        return k_bishop_magic_table[location.offset][key];
    }

    void generate_pawn_moves(const Helper& helper, std::vector<Move>& moves)
    {
        Piece piece = helper.piece_to_move(Piece::Type::Pawn);
        BitBoard positions = helper.occupancy(Piece::Type::Pawn);
        positions &= helper.non_occupancy();

        BitBoard promotion_positions = positions & helper.backrank_mask();
        BitBoard non_promotion_positions = positions & ~helper.backrank_mask();

        // Single step forward moves
        while (non_promotion_positions.any()) {
            auto location = non_promotion_positions.pop_lsb().value();
            moves.push_back(Move::by_moving(piece, location, helper.forward(location)));
        }
    }

    void generate_legal_moves(const MoveGenerator::Request& request, std::vector<Move>& moves)
    {
        Helper helper(request);
        generate_pawn_moves(helper, moves);
    }
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
    Result result {};
    return result;
}

namespace testapi {

    BitBoard rook_attacks(Location location, BitBoard blockers) { return generate_rook_attacks(location, blockers); }

    BitBoard bishop_attacks(Location location, BitBoard blockers)
    {
        return generate_bishop_attacks(location, blockers);
    }
}

} // namespace weechess
