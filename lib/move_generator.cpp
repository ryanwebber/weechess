#include <array>

#include <weechess/bit_board.h>
#include <weechess/comptime.h>
#include <weechess/move_generator.h>

#include "log.h"

namespace weechess {

namespace {

    constexpr std::array<Piece::Type, 4> promotion_types = {
        Piece::Type::Queen,
        Piece::Type::Rook,
        Piece::Type::Bishop,
        Piece::Type::Knight,
    };

    const std::array<BitBoard, 64> k_knight_attacks = comptime::compute_knight_attacks();
    const std::array<BitBoard, 64> k_king_attacks = comptime::compute_king_attacks();

    const comptime::RookMagicTable k_rook_magic_table = comptime::compute_rook_magic_table();
    const comptime::BishopMagicTable k_bishop_magic_table = comptime::compute_bishop_magic_table();

    const std::array<BitBoard, 64> k_rook_masks = comptime::compute_rook_slide_masks();
    const std::array<BitBoard, 64> k_bishop_masks = comptime::compute_bishop_slide_masks();

    class Helper {
    private:
        const MoveGenerator::Request& m_request;

    public:
        Helper(const MoveGenerator::Request& request)
            : m_request(request)
        {
        }

        const Board& board() const { return m_request.board(); }

        Color color_to_move() const { return m_request.turn_to_move(); }
        Piece piece_to_move(Piece::Type type) const { return Piece(type, m_request.turn_to_move()); }

        std::optional<Location> en_passant_target() const { return m_request.en_passant_target(); }

        CastleRights castle_rights_to_move() const { return m_request.castle_rights()[color_to_move()]; }

        Location forward(Location location) const
        {
            if (m_request.turn_to_move() == Color::White)
                return location.offset_by(Location::Up).value();
            else
                return location.offset_by(Location::Down).value();
        }

        Location backward(Location location) const
        {
            if (m_request.turn_to_move() == Color::White)
                return location.offset_by(Location::Down).value();
            else
                return location.offset_by(Location::Up).value();
        }

        Location file_shifted(Location location, int8_t sign) const
        {
            if ((m_request.turn_to_move() == Color::White) == (sign > 0))
                return location.offset_by(Location::Right).value();
            else
                return location.offset_by(Location::Left).value();
        }

        BitBoard file_shifted(BitBoard bb, int8_t sign) const
        {
            if ((m_request.turn_to_move() == Color::White) == (sign > 0))
                return (bb & ~comptime::file_mask[comptime::H_File]) << 1;
            else
                return (bb & ~comptime::file_mask[comptime::A_File]) >> 1;
        }

        BitBoard backrank_mask() const
        {
            if (m_request.turn_to_move() == Color::White)
                return comptime::rank_mask[comptime::_8th_Rank];
            else
                return comptime::rank_mask[comptime::_1st_Rank];
        }

        BitBoard home_rank_mask(int rank) const
        {
            if (m_request.turn_to_move() == Color::White)
                return comptime::rank_mask[comptime::_1st_Rank] << (8 * (rank - 1));
            else
                return comptime::rank_mask[comptime::_8th_Rank] >> (8 * (rank - 1));
        }

        BitBoard edge_file_mask(int sign) const
        {
            if ((m_request.turn_to_move() == Color::White) == (sign > 0))
                return comptime::file_mask[comptime::H_File];
            else
                return comptime::file_mask[comptime::A_File];
        }

        BitBoard shift_forward(BitBoard bb) const
        {
            if (m_request.turn_to_move() == Color::White)
                return bb << 8;
            else
                return bb >> 8;
        }

        BitBoard occupancy_to_move(Piece::Type type) const
        {
            return m_request.board().occupancy_for(Piece(type, m_request.turn_to_move()));
        }

        BitBoard attackable() const
        {
            auto other_color = invert_color(m_request.turn_to_move());
            return m_request.board().color_occupancy()[other_color];
        }

        BitBoard en_passant_mask() const
        {
            BitBoard bb;
            if (m_request.en_passant_target().has_value()) {
                bb.set(m_request.en_passant_target().value());
            }

            return bb;
        }
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

    void expand_moves(
        const Helper& helper, std::vector<Move>& moves, Location origin, BitBoard targets, Piece::Type type)
    {
        auto piece = helper.piece_to_move(type);
        auto attacks = helper.attackable() & targets;
        while (attacks.any()) {
            auto target = attacks.pop_lsb().value();
            auto move = Move::by_capturing(piece, origin, target, helper.board().piece_at(target).type());
            moves.push_back(move);
        }

        auto non_attacks = ~helper.attackable() & targets;
        while (non_attacks.any()) {
            auto target = non_attacks.pop_lsb().value();
            auto move = Move::by_moving(piece, origin, target);
            moves.push_back(move);
        }
    }

    void generate_pawn_moves(const Helper& helper, std::vector<Move>& moves)
    {

        Piece piece = helper.piece_to_move(Piece::Type::Pawn);

        // Single step forward moves
        {
            BitBoard positions = helper.shift_forward(helper.occupancy_to_move(Piece::Type::Pawn));
            positions &= helper.board().non_occupancy();

            BitBoard promotion_positions = positions & helper.backrank_mask();
            BitBoard non_promotion_positions = positions & ~helper.backrank_mask();

            // Non-promotion moves
            while (non_promotion_positions.any()) {
                auto target = non_promotion_positions.pop_lsb().value();
                auto move = Move::by_moving(piece, helper.backward(target), target);
                moves.push_back(move);
            }

            // Promotion moves
            while (promotion_positions.any()) {
                auto target = promotion_positions.pop_lsb().value();
                for (const auto& type : promotion_types) {
                    moves.push_back(Move::by_promoting(piece, helper.backward(target), target, type));
                }
            }
        }

        // Two steps froward
        {
            BitBoard pawns = helper.occupancy_to_move(Piece::Type::Pawn) & helper.home_rank_mask(2);
            BitBoard non_occupancy = helper.board().non_occupancy();
            BitBoard single_moves = helper.shift_forward(pawns) & non_occupancy;
            BitBoard double_moves = helper.shift_forward(single_moves) & non_occupancy;
            while (double_moves.any()) {
                auto target = double_moves.pop_lsb().value();
                auto move = Move::by_moving(piece, helper.backward(helper.backward(target)), target);
                move.set_double_pawn_push();
                moves.push_back(move);
            }
        }

        // Captures
        {
            BitBoard pawns = helper.occupancy_to_move(Piece::Type::Pawn);
            constexpr std::array<int8_t, 2> signs = { -1, 1 };
            for (const auto& sign : signs) {
                BitBoard attacks = helper.file_shifted(helper.shift_forward(pawns), sign) & helper.attackable();
                BitBoard attacks_with_promotion = attacks & helper.backrank_mask();
                BitBoard en_passant_attacks
                    = helper.file_shifted(helper.shift_forward(pawns), sign) & helper.en_passant_mask();

                attacks &= ~helper.backrank_mask();

                // Regular captures
                while (attacks.any()) {
                    auto target = attacks.pop_lsb().value();
                    auto origin = helper.backward(helper.file_shifted(target, -sign));
                    auto move = Move::by_capturing(piece, origin, target, helper.board().piece_at(target).type());
                    moves.push_back(move);
                }

                // Promotion captures
                while (attacks_with_promotion.any()) {
                    auto target = attacks_with_promotion.pop_lsb().value();
                    auto capture = helper.board().piece_at(target).type();
                    for (const auto& type : promotion_types) {
                        auto move = Move::by_promoting(
                            piece, helper.backward(helper.file_shifted(target, -sign)), target, type);
                        move.set_capture(capture);
                        moves.push_back(move);
                    }
                }

                // En passant captures
                if (en_passant_attacks.any()) {
                    auto target = helper.en_passant_target().value();
                    moves.push_back(
                        Move::by_en_passant(piece, helper.backward(helper.file_shifted(target, -sign)), target));
                }
            }
        }
    }

    void generate_knight_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto knights = helper.occupancy_to_move(Piece::Type::Knight);
        while (knights.any()) {
            auto origin = knights.pop_lsb().value();
            auto jumps = k_knight_attacks[origin.offset] & (helper.attackable() | helper.board().non_occupancy());
            expand_moves(helper, moves, origin, jumps, Piece::Type::Knight);
        }
    }

    void generate_king_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto kings = helper.occupancy_to_move(Piece::Type::King);
        while (kings.any()) {
            auto origin = kings.pop_lsb().value();
            auto jumps = k_king_attacks[origin.offset] & (helper.attackable() | helper.board().non_occupancy());
            expand_moves(helper, moves, origin, jumps, Piece::Type::King);
        }

        // TODO: Check checks and piece validity
        return;

        if (helper.castle_rights_to_move().can_castle_kingside) {
            moves.push_back(Move::by_castling(helper.piece_to_move(Piece::Type::King), CastleSide::Kingside));
        }

        if (helper.castle_rights_to_move().can_castle_queenside) {
            moves.push_back(Move::by_castling(helper.piece_to_move(Piece::Type::King), CastleSide::Queenside));
        }
    }

    void generate_bishop_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto occupancy = helper.board().shared_occupancy();
        auto bishops = helper.occupancy_to_move(Piece::Type::Bishop);
        auto own_pieces = helper.board().color_occupancy()[helper.color_to_move()];
        while (bishops.any()) {
            auto origin = bishops.pop_lsb().value();
            auto attacks = generate_bishop_attacks(origin, occupancy);
            auto slides = attacks & ~own_pieces;
            expand_moves(helper, moves, origin, slides, Piece::Type::Bishop);
        }
    }

    void generate_rook_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto occupancy = helper.board().shared_occupancy();
        auto rooks = helper.occupancy_to_move(Piece::Type::Rook);
        auto own_pieces = helper.board().color_occupancy()[helper.color_to_move()];
        while (rooks.any()) {
            auto origin = rooks.pop_lsb().value();
            auto attacks = generate_rook_attacks(origin, occupancy);
            auto slides = attacks & ~own_pieces;
            expand_moves(helper, moves, origin, slides, Piece::Type::Rook);
        }
    }

    void generate_queen_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto occupancy = helper.board().shared_occupancy();
        auto queens = helper.occupancy_to_move(Piece::Type::Queen);
        auto own_pieces = helper.board().color_occupancy()[helper.color_to_move()];
        while (queens.any()) {
            auto origin = queens.pop_lsb().value();
            auto attacks = generate_bishop_attacks(origin, occupancy) | generate_rook_attacks(origin, occupancy);
            auto slides = attacks & ~own_pieces;
            expand_moves(helper, moves, origin, slides, Piece::Type::Queen);
        }
    }

    void generate_legal_moves(const MoveGenerator::Request& request, std::vector<Move>& moves)
    {
        Helper helper(request);
        generate_pawn_moves(helper, moves);
        generate_knight_moves(helper, moves);
        generate_king_moves(helper, moves);
        generate_bishop_moves(helper, moves);
        generate_rook_moves(helper, moves);
        generate_queen_moves(helper, moves);
    }
}

MoveGenerator::Request::Request(Board board)
    : m_board(board)
    , m_turn_to_move(Color::White)
    , m_castle_rights(CastleRights::all())
    , m_en_passant_target({})
{
}

void MoveGenerator::Request::set_turn_to_move(Color color) { m_turn_to_move = color; }
void MoveGenerator::Request::set_castle_rights(Color color, CastleRights rights) { m_castle_rights[color] = rights; }
void MoveGenerator::Request::set_en_passant_target(const Location& location) { m_en_passant_target = location; }

MoveGenerator::Result MoveGenerator::execute(const Request& request) const
{
    Result result;
    generate_legal_moves(request, result.legal_moves);
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
