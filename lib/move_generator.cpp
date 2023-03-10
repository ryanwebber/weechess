#include <array>

#include <weechess/attack_maps.h>
#include <weechess/bit_board.h>
#include <weechess/move_generator.h>

#include "log.h"

namespace weechess {

namespace {

    namespace castling {
        constexpr ColorMap<BitBoard> kingside_check_mask(BitBoard(0x70ULL), BitBoard(0x7000000000000000ULL));
        constexpr ColorMap<BitBoard> queenside_check_mask(BitBoard(0x1cULL), BitBoard(0x1c00000000000000ULL));
        constexpr ColorMap<BitBoard> kingside_path_mask(BitBoard(0x60ULL), BitBoard(0x6000000000000000ULL));
        constexpr ColorMap<BitBoard> queenside_path_mask(BitBoard(0xeULL), BitBoard(0xe00000000000000ULL));
    }

    constexpr std::array<Piece::Type, 4> promotion_types = {
        Piece::Type::Queen,
        Piece::Type::Rook,
        Piece::Type::Bishop,
        Piece::Type::Knight,
    };

    class Helper {
    private:
        const GameSnapshot& m_snapshot;

    public:
        Helper(const GameSnapshot& snapshot)
            : m_snapshot(snapshot)
        {
        }

        const Board& board() const { return m_snapshot.board; }

        Color color_to_move() const { return m_snapshot.turn_to_move; }
        Piece piece_to_move(Piece::Type type) const { return Piece(type, m_snapshot.turn_to_move); }

        std::optional<Location> en_passant_target() const { return m_snapshot.en_passant_target; }

        CastleRights castle_rights_to_move() const { return m_snapshot.castle_rights[color_to_move()]; }

        Location forward(Location location) const
        {
            if (m_snapshot.turn_to_move == Color::White)
                return location.offset_by(Location::Up).value();
            else
                return location.offset_by(Location::Down).value();
        }

        Location backward(Location location) const
        {
            if (m_snapshot.turn_to_move == Color::White)
                return location.offset_by(Location::Down).value();
            else
                return location.offset_by(Location::Up).value();
        }

        Location file_shifted(Location location, int8_t sign) const
        {
            if ((m_snapshot.turn_to_move == Color::White) == (sign > 0))
                return location.offset_by(Location::Right).value();
            else
                return location.offset_by(Location::Left).value();
        }

        BitBoard file_shifted(BitBoard bb, int8_t sign) const
        {
            if ((m_snapshot.turn_to_move == Color::White) == (sign > 0))
                return (bb & ~File('H').mask()) << 1;
            else
                return (bb & ~File('A').mask()) >> 1;
        }

        BitBoard backrank_mask() const
        {
            if (m_snapshot.turn_to_move == Color::White)
                return Rank(8).mask();
            else
                return Rank(1).mask();
        }

        BitBoard home_rank_mask(Rank rank) const
        {
            if (m_snapshot.turn_to_move == Color::White)
                return rank.mask();
            else
                return rank.inverted().mask();
        }

        BitBoard edge_file_mask(int sign) const
        {
            if ((m_snapshot.turn_to_move == Color::White) == (sign > 0))
                return File('H').mask();
            else
                return File('A').mask();
        }

        BitBoard shift_forward(BitBoard bb) const
        {
            if (m_snapshot.turn_to_move == Color::White)
                return bb << 8;
            else
                return bb >> 8;
        }

        BitBoard occupancy_to_move(Piece::Type type) const
        {
            return m_snapshot.board.occupancy_for(Piece(type, m_snapshot.turn_to_move));
        }

        BitBoard attackable() const
        {
            auto other_color = invert_color(m_snapshot.turn_to_move);
            return m_snapshot.board.color_occupancy()[other_color];
        }

        BitBoard threats() const
        {
            auto other_color = invert_color(m_snapshot.turn_to_move);
            return m_snapshot.board.attacks(other_color);
        }

        BitBoard en_passant_mask() const
        {
            BitBoard bb;
            if (m_snapshot.en_passant_target.has_value()) {
                bb.set(m_snapshot.en_passant_target.value());
            }

            return bb;
        }
    };

    void expand_moves(
        const Helper& helper, std::vector<Move>& moves, Location origin, BitBoard targets, Piece::Type type)
    {
        auto piece = helper.piece_to_move(type);
        auto attacks = helper.attackable() & targets;
        while (attacks.any()) {
            auto target = attacks.pop_lsb().value();
            auto move = Move::by_capturing(piece, origin, target, helper.board().piece_at(target).type);
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
            BitBoard pawns = helper.occupancy_to_move(Piece::Type::Pawn) & helper.home_rank_mask(Rank(2));
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
                    auto move = Move::by_capturing(piece, origin, target, helper.board().piece_at(target).type);
                    moves.push_back(move);
                }

                // Promotion captures
                while (attacks_with_promotion.any()) {
                    auto target = attacks_with_promotion.pop_lsb().value();
                    auto capture = helper.board().piece_at(target).type;
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
            auto jumps
                = attack_maps::generate_knight_attacks(origin) & (helper.attackable() | helper.board().non_occupancy());
            expand_moves(helper, moves, origin, jumps, Piece::Type::Knight);
        }
    }

    void generate_king_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto color = helper.color_to_move();
        auto kings = helper.occupancy_to_move(Piece::Type::King);
        while (kings.any()) {
            auto origin = kings.pop_lsb().value();
            auto jumps = attack_maps::generate_king_attacks(origin)
                & (helper.attackable() | helper.board().non_occupancy()) & ~helper.threats();
            expand_moves(helper, moves, origin, jumps, Piece::Type::King);
        }

        if (helper.castle_rights_to_move().can_castle_kingside) {
            auto path_blocks = helper.board().shared_occupancy() & castling::kingside_path_mask[color];
            auto path_checks = helper.threats() & castling::kingside_check_mask[color];
            if (path_blocks.none() && path_checks.none()) {
                moves.push_back(Move::by_castling(helper.piece_to_move(Piece::Type::King), CastleSide::Kingside));
            }
        }

        if (helper.castle_rights_to_move().can_castle_queenside) {
            auto path_blocks = helper.board().shared_occupancy() & castling::queenside_path_mask[color];
            auto path_checks = helper.threats() & castling::queenside_check_mask[color];
            if (path_blocks.none() && path_checks.none()) {
                moves.push_back(Move::by_castling(helper.piece_to_move(Piece::Type::King), CastleSide::Queenside));
            }
        }
    }

    void generate_bishop_moves(const Helper& helper, std::vector<Move>& moves)
    {
        auto occupancy = helper.board().shared_occupancy();
        auto bishops = helper.occupancy_to_move(Piece::Type::Bishop);
        auto own_pieces = helper.board().color_occupancy()[helper.color_to_move()];
        while (bishops.any()) {
            auto origin = bishops.pop_lsb().value();
            auto attacks = attack_maps::generate_bishop_attacks(origin, occupancy);
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
            auto attacks = attack_maps::generate_rook_attacks(origin, occupancy);
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
            auto attacks = attack_maps::generate_queen_attacks(origin, occupancy);
            auto slides = attacks & ~own_pieces;
            expand_moves(helper, moves, origin, slides, Piece::Type::Queen);
        }
    }

    void generate_psuedo_legal_moves(const GameSnapshot& snapshot, std::vector<Move>& moves)
    {
        Helper helper(snapshot);
        generate_pawn_moves(helper, moves);
        generate_knight_moves(helper, moves);
        generate_king_moves(helper, moves);
        generate_bishop_moves(helper, moves);
        generate_rook_moves(helper, moves);
        generate_queen_moves(helper, moves);
    }
}

MoveGenerator::Result MoveGenerator::execute(const GameSnapshot& snapshot) const
{
    Result result;

    // Reserve a reasonable amount of space for the legal moves
    // to avoid reallocations.
    std::vector<Move> moves;
    moves.reserve(128);

    generate_psuedo_legal_moves(snapshot, moves);

    result.legal_moves.reserve(moves.size());
    for (const auto& move : moves) {
        auto new_snapshot = snapshot.by_performing_move(move);
        auto king_position = new_snapshot->board.occupancy_for(Piece(Piece::Type::King, snapshot.turn_to_move));
        auto attacked_positions = new_snapshot->board.attacks(new_snapshot->turn_to_move);

        if ((king_position & attacked_positions).none()) {
            result.legal_moves.emplace_back(move, std::move(*new_snapshot));
        }
    }

    return result;
}

} // namespace weechess
