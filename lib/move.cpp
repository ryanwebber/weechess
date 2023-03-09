#include <functional>

#include <weechess/game_state.h>
#include <weechess/move.h>

namespace weechess {

const Move Move::null {};

Move::Data Move::data() const { return m_data; }

Piece Move::moving_piece() const
{
    auto piece_type = static_cast<Piece::Type>(get_flags(Flags::PieceType));
    return Piece(piece_type, color());
}

Piece Move::resulting_piece() const
{
    if (is_promotion()) {
        auto piece_type = promoted_piece_type();
        return Piece(piece_type, color());
    } else {
        auto piece_type = static_cast<Piece::Type>(get_flags(Flags::PieceType));
        return Piece(piece_type, color());
    }
}

uint32_t Move::get_flags(Flags flags) const
{
    return (m_data.to_ulong() & masks[static_cast<uint32_t>(flags)]) >> shifts[static_cast<uint32_t>(flags)];
}

Color Move::color() const { return get_flags(Flags::Color) == 1 ? Color::White : Color::Black; }

void Move::set_flags(Flags flags, uint32_t value)
{
    m_data &= ~masks[static_cast<uint32_t>(flags)];
    m_data |= ((value << shifts[static_cast<uint32_t>(flags)]) & masks[static_cast<uint32_t>(flags)]);
}

void Move::set_color(Color color) { set_flags(Flags::Color, color == Color::White ? 1 : 0); }
void Move::set_piece_type(Piece::Type type) { set_flags(Flags::PieceType, static_cast<uint32_t>(type)); }
void Move::set_origin(Location location) { set_flags(Flags::Origin, location.offset); }
void Move::set_destination(Location location) { set_flags(Flags::Destination, location.offset); }
void Move::set_capture(Piece::Type type) { set_flags(Flags::Capture, static_cast<uint32_t>(type)); }
void Move::set_double_pawn_push() { set_flags(Flags::DoublePawn, 1); }
void Move::set_promotion(Piece::Type type)
{
    if (type == Piece::Type::None) {
        set_flags(Flags::Promotion, 0b0000);
    } else {
        set_flags(Flags::Promotion, 0b1000 | static_cast<uint32_t>(type));
    }
}

Location Move::start_location() const { return Location(get_flags(Flags::Origin)); }
Location Move::end_location() const { return Location(get_flags(Flags::Destination)); }

bool Move::is_capture() const { return get_flags(Flags::Capture) != 0; }
bool Move::is_en_passant() const { return get_flags(Flags::EnPassant) != 0; }
bool Move::is_double_pawn() const { return get_flags(Flags::DoublePawn) != 0; }
bool Move::is_promotion() const { return (get_flags(Flags::Promotion) & 0b1000) != 0; }
bool Move::is_castle() const
{
    return (get_flags(Flags::KingsideCastle) != 0) || (get_flags(Flags::QueensideCastle) != 0);
}

Piece::Type Move::captured_piece_type() const { return static_cast<Piece::Type>(get_flags(Flags::Capture)); }
Piece::Type Move::promoted_piece_type() const { return static_cast<Piece::Type>(0b0111 & get_flags(Flags::Promotion)); }

std::optional<CastleSide> Move::castle_side() const
{
    if (get_flags(Flags::KingsideCastle) != 0) {
        return CastleSide::Kingside;
    } else if (get_flags(Flags::QueensideCastle) != 0) {
        return CastleSide::Queenside;
    } else {
        return {};
    }
}

Move Move::by_moving(Piece piece, Location from, Location to)
{
    Move move;
    move.set_piece_type(piece.type);
    move.set_origin(from);
    move.set_destination(to);
    move.set_color(piece.color);
    return move;
}

Move Move::by_capturing(Piece piece, Location from, Location to, Piece::Type captured)
{
    Move move = by_moving(piece, from, to);
    move.set_capture(captured);
    return move;
}

Move Move::by_promoting(Piece piece, Location from, Location to, Piece::Type promotion)
{
    Move move = by_moving(piece, from, to);
    move.set_promotion(promotion);
    return move;
}

Move Move::by_en_passant(Piece piece, Location from, Location to)
{
    Move move = by_moving(piece, from, to);
    move.set_flags(Flags::EnPassant, 1);
    move.set_capture(Piece::Type::Pawn);
    return move;
}

Move Move::by_castling(Piece piece, CastleSide side)
{
    Move move;
    move.set_piece_type(piece.type);
    move.set_color(piece.color);
    if (side == CastleSide::Kingside) {
        move.set_flags(Flags::KingsideCastle, 1);
        if (piece.color == Color::White) {
            move.set_origin(Location::E1);
            move.set_destination(Location::G1);
        } else {
            move.set_origin(Location::E8);
            move.set_destination(Location::G8);
        }
    } else {
        move.set_flags(Flags::QueensideCastle, 1);
        if (piece.color == Color::White) {
            move.set_origin(Location::E1);
            move.set_destination(Location::C1);
        } else {
            move.set_origin(Location::E8);
            move.set_destination(Location::C8);
        }
    }

    return move;
}

std::string Move::san_notation(const GameState& gs) const { return gs.san_notation(*this); }
std::string Move::to_string() const { return start_location().to_string() + end_location().to_string(); }

std::size_t MoveHash::operator()(const Move& move) const { return std::hash<Move::Data> {}(move.m_data); }

bool operator==(const Move& lhs, const Move& rhs) { return lhs.m_data == rhs.m_data; }
bool operator!=(const Move& lhs, const Move& rhs) { return !(lhs == rhs); }
bool operator<(const Move& lhs, const Move& rhs) { return lhs.m_data.to_ulong() < rhs.m_data.to_ulong(); }

} // namespace weechess
