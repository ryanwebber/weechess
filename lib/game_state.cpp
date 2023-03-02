#include <sstream>

#include "fen.h"
#include "log.h"
#include <weechess/game_state.h>
#include <weechess/move_generator.h>

namespace weechess {

bool CastleRights::has_rights() const { return can_castle_kingside || can_castle_queenside; }

GameSnapshot::GameSnapshot(Board board,
    Color turn_to_move,
    ColorMap<CastleRights> castle_rights,
    std::optional<Location> en_passant_target,
    size_t halfmove_clock,
    size_t fullmove_number)
    : board(std::move(board))
    , turn_to_move(turn_to_move)
    , castle_rights(castle_rights)
    , en_passant_target(en_passant_target)
    , halfmove_clock(halfmove_clock)
    , fullmove_number(fullmove_number)
{
}

std::optional<GameSnapshot> GameSnapshot::by_performing_move(const Move& move) const
{
    return GameSnapshot::by_performing_move(*this, move);
}

std::optional<GameSnapshot> GameSnapshot::by_performing_moves(std::span<const std::shared_ptr<MoveQuery>> moves) const
{
    return GameSnapshot::by_performing_moves(*this, moves);
}

std::string GameSnapshot::to_fen() const { return fen::to_fen(*this); }

std::optional<GameSnapshot> GameSnapshot::from_fen(std::string_view fen_sv) { return fen::from_fen(fen_sv); }

GameSnapshot GameSnapshot::initial_position() { return fen::from_fen(fen::initial_position).value(); }

LegalMove::LegalMove(Move move, GameSnapshot snapshot)
    : m_move(std::move(move))
    , m_snapshot(std::move(snapshot))
{
}

const Move& LegalMove::move() const { return m_move; }
const GameSnapshot& LegalMove::snapshot() const { return m_snapshot; }

const Move& LegalMove::operator*() const { return m_move; }
const Move* LegalMove::operator->() const { return &m_move; }

MoveSet::MoveSet(std::vector<LegalMove> legal_moves)
    : m_legal_moves(std::move(legal_moves))
{
}

std::span<const LegalMove> MoveSet::legal_moves() const { return m_legal_moves; };
std::vector<LegalMove> MoveSet::legal_moves_from(Location from) const
{
    LocationMoveQuery query(from);
    return find(query);
}

std::vector<LegalMove> MoveSet::find(const MoveQuery& query) const
{
    std::vector<LegalMove> result;
    for (const auto& legal_move : m_legal_moves) {
        if (query.test(legal_move.move())) {
            result.push_back(legal_move);
        }
    }

    return result;
}

std::optional<LegalMove> MoveSet::find_first(const MoveQuery& query) const
{
    auto iter = std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [&query](const auto& legal_move) {
        return query.test(legal_move.move());
    });

    if (iter == m_legal_moves.end()) {
        return {};
    }

    return *iter;
}

std::optional<LegalMove> MoveSet::find(const Move& move) const
{
    auto iter = std::find_if(m_legal_moves.begin(), m_legal_moves.end(), [&move](const auto& legal_move) {
        return legal_move.move() == move;
    });

    if (iter == m_legal_moves.end()) {
        return {};
    }

    return *iter;
}

MoveSet MoveSet::compute(const GameSnapshot& snapshot)
{
    MoveGenerator generator;
    auto result = generator.execute(snapshot);
    return MoveSet(std::move(result.legal_moves));
}

GameState::GameState()
    : m_snapshot({})
{
}

GameState::GameState(GameSnapshot snapshot)
    : m_snapshot(std::move(snapshot))
{
}

const Color& GameState::turn_to_move() const { return m_snapshot.turn_to_move; }
const ColorMap<CastleRights>& GameState::castle_rights() const { return m_snapshot.castle_rights; }
const std::optional<Location>& GameState::en_passant_target() const { return m_snapshot.en_passant_target; }

size_t GameState::halfmove_clock() const { return m_snapshot.halfmove_clock; }
size_t GameState::fullmove_number() const { return m_snapshot.fullmove_number; }

bool GameState::is_check() const { return false; }
bool GameState::is_checkmate() const { return move_set().legal_moves().empty() && is_check(); }
bool GameState::is_stalemate() const { return move_set().legal_moves().empty() && !is_check(); }

const GameSnapshot& GameState::snapshot() const { return m_snapshot; }

const Board& GameState::board() const { return m_snapshot.board; }
const MoveSet& GameState::move_set() const
{
    if (!m_move_set.has_value()) {
        auto mut_this = const_cast<GameState*>(this);
        mut_this->m_move_set = MoveSet::compute(m_snapshot);
    }

    return m_move_set.value();
}

std::string GameState::san_notation(const Move& move) const
{
    auto target = move.end_location();
    auto origin = move.start_location();
    auto piece = move.moving_piece();

    std::stringstream ss;

    if (piece.type == Piece::Type::Pawn && move.is_capture()) {
        ss << static_cast<char>('a' + origin.file());
    } else {
        ss << static_cast<char>(std::toupper(piece.to_letter()));

        std::vector<Location> alternative_possible_sources;
        for (const auto& legal_move : move_set().legal_moves()) {
            const auto& move = legal_move.move();
            if (move.end_location() == target && move.start_location() != origin && move.moving_piece() == piece) {
                alternative_possible_sources.push_back(move.start_location());
            }
        }

        if (alternative_possible_sources.size() == 1) {
            if (alternative_possible_sources[0].file() != origin.file()) {
                ss << static_cast<char>('a' + origin.file());
            } else {
                ss << (origin.rank() + 1);
            }
        } else if (alternative_possible_sources.size() > 1) {
            ss << origin.to_string();
        }
    }

    if (move.is_capture()) {
        ss << 'x';
    }

    ss << target.to_string();

    return ss.str();
}

std::string GameState::verbose_description(const Move& move) const
{
    std::stringstream ss;

    auto legal_move = move_set().find(move);
    auto move_from = move.start_location();
    auto move_to = move.end_location();

    ss << "\n";

    for (auto i = 0; i < 8; i++) {
        auto rank = 7 - i;
        ss << (rank + 1) << "   ";

        for (auto file = 0; file < 8; file++) {
            auto loc = Location::from_rank_and_file(rank, file);

            auto piece = board().piece_at(loc);
            auto letter = piece.exists() ? piece.to_letter() : '.';

            if (loc == move_from) {
                ss << '[' << letter << ']';
            } else {
                ss << ' ' << letter << ' ';
            }
        }

        if (rank == 3) {
            ss << "     →     ";
        } else {
            ss << "           ";
        }

        if (legal_move.has_value()) {
            for (auto file = 0; file < 8; file++) {
                auto loc = Location::from_rank_and_file(rank, file);

                auto piece = legal_move->snapshot().board.piece_at(loc);
                auto letter = piece.exists() ? piece.to_letter() : '.';

                if (loc == move_from || loc == move_to) {
                    ss << '[' << letter << ']';
                } else {
                    ss << ' ' << letter << ' ';
                }
            }
        } else if (rank == 3) {
            ss << " .  . Invalid Move .  . ";
        } else {
            ss << " .  .  .  .  .  .  .  . ";
        }

        ss << "\n\n";
    }

    ss << "\n";
    ss << "   A  B  C  D  E  F  G  H              A  B  C  D  E  F  G  H";
    ss << "\n";
    ss << "\n";

    ss << "Flags\n";
    ss << "Move:       " << san_notation(move) << "\n";

    ss << "Capture:    ";
    if (move.is_capture()) {
        ss << Piece(move.captured_piece_type(), invert_color(m_snapshot.turn_to_move)).to_letter();
    } else {
        ss << "-";
    }
    ss << "\n";

    ss << "Promotion:  ";
    if (move.is_promotion()) {
        ss << Piece(move.promoted_piece_type(), m_snapshot.turn_to_move).to_letter();
    } else {
        ss << "-";
    }
    ss << "\n";

    ss << "Castle:     ";
    if (move.is_castle() && move.castle_side() == CastleSide::Kingside) {
        ss << "kingside";
    } else if (move.is_castle() && move.castle_side() == CastleSide::Queenside) {
        ss << "queenside";
    } else {
        ss << "-";
    }
    ss << "\n";

    ss << "En Passant: ";
    if (move.is_en_passant()) {
        ss << "yes";
    } else {
        ss << "no";
    }

    return ss.str();
}

std::string GameState::to_fen() const { return m_snapshot.to_fen(); }
std::optional<GameState> GameState::from_fen(std::string_view fen_sv)
{
    auto snapshot = GameSnapshot::from_fen(fen_sv);
    if (!snapshot.has_value())
        return {};
    return GameState(*snapshot);
}

GameState GameState::new_game() { return GameState(GameSnapshot::initial_position()); }

std::optional<GameSnapshot> GameSnapshot::by_performing_move(const GameSnapshot& snapshot, const Move& move)
{
    auto other_color = invert_color(snapshot.turn_to_move);

    auto board = GameSnapshot::augmented_board_for_move(snapshot, move);
    if (!board)
        return {};

    auto halfmove_clock = snapshot.halfmove_clock + 1;
    if (move.is_capture() || move.moving_piece().type == Piece::Type::Pawn) {
        halfmove_clock = 0;
    }

    auto fullmove_number = snapshot.fullmove_number;
    if (snapshot.turn_to_move == Color::Black) {
        fullmove_number++;
    }

    auto en_passant_target = std::optional<Location>();
    if (move.is_double_pawn()) {
        auto rank_offset = snapshot.turn_to_move == Color::White ? 1 : -1;
        en_passant_target = move.start_location().offset_by(Location::RankShift { rank_offset });
    }

    auto castle_rights = snapshot.castle_rights;
    if (move.moving_piece().type == Piece::Type::King) {
        castle_rights[snapshot.turn_to_move].can_castle_kingside = false;
        castle_rights[snapshot.turn_to_move].can_castle_queenside = false;
    }

    castle_rights[Color::Black].can_castle_kingside
        &= board->occupancy_for(Piece(Piece::Type::Rook, Color::Black))[Location::H8];
    castle_rights[Color::Black].can_castle_queenside
        &= board->occupancy_for(Piece(Piece::Type::Rook, Color::Black))[Location::A8];
    castle_rights[Color::White].can_castle_kingside
        &= board->occupancy_for(Piece(Piece::Type::Rook, Color::White))[Location::H1];
    castle_rights[Color::White].can_castle_queenside
        &= board->occupancy_for(Piece(Piece::Type::Rook, Color::White))[Location::A1];

    return GameSnapshot(
        std::move(*board), other_color, castle_rights, en_passant_target, halfmove_clock, fullmove_number);
}

std::optional<GameSnapshot> GameSnapshot::by_performing_moves(
    const GameSnapshot& snapshot, std::span<const std::shared_ptr<MoveQuery>> moves)
{
    std::optional<GameSnapshot> gs = snapshot;
    auto moves_to_make = moves.begin();
    while (gs.has_value() && moves_to_make != moves.end()) {
        auto move_set = MoveSet::compute(*gs);
        if (move_set.legal_moves().empty()) {
            return {};
        }

        gs = move_set.legal_moves()[0].snapshot();
        moves_to_make++;
    }

    return gs;
}

std::optional<Board> GameSnapshot::augmented_board_for_move(const GameSnapshot& snapshot, const Move& move)
{
    auto moving_piece = move.moving_piece();
    auto resulting_piece = move.resulting_piece();
    auto color = move.color();
    auto other_color = invert_color(color);

    auto buffer = snapshot.board.piece_buffer();

    // Start and end positions of the piece
    buffer.occupancy_for(moving_piece).unset(move.start_location());
    buffer.occupancy_for(resulting_piece).set(move.end_location());

    if (move.is_en_passant()) {
        if (!snapshot.en_passant_target.has_value()) {
            log::error("Move is an en passant move, but no en passant target is set");
            return {};
        }

        auto captured_piece = Piece(Piece::Type::Pawn, other_color);
        if (other_color == Color::Black) {
            auto removed_location = snapshot.en_passant_target->offset_by(Location::RankShift { -1 }).value();
            buffer.occupancy_for(captured_piece).unset(removed_location);
        } else {
            auto removed_location = snapshot.en_passant_target->offset_by(Location::RankShift { 1 }).value();
            buffer.occupancy_for(captured_piece).unset(removed_location);
        }
    } else if (move.is_capture()) {
        auto captured_piece = Piece(move.captured_piece_type(), other_color);
        buffer.occupancy_for(captured_piece).unset(move.end_location());
    }

    if (move.is_promotion()) {
        buffer.occupancy_for(moving_piece).unset(move.end_location());
        buffer.occupancy_for(resulting_piece).set(move.end_location());
    }

    if (move.is_castle()) {
        if (move.castle_side() == CastleSide::Kingside) {
            auto rook_start = Location::from_rank_and_file(move.start_location().rank(), 7);
            auto rook_end = Location::from_rank_and_file(move.start_location().rank(), 5);
            auto rook = Piece(Piece::Type::Rook, color);
            buffer.occupancy_for(rook).unset(rook_start);
            buffer.occupancy_for(rook).set(rook_end);
        } else {
            auto rook_start = Location::from_rank_and_file(move.start_location().rank(), 0);
            auto rook_end = Location::from_rank_and_file(move.start_location().rank(), 3);
            auto rook = Piece(Piece::Type::Rook, color);
            buffer.occupancy_for(rook).unset(rook_start);
            buffer.occupancy_for(rook).set(rook_end);
        }
    }

    return Board(std::move(buffer));
}

} // namespace weechess
