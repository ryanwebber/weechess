#include <weechess/move.h>

namespace weechess {

Move::Move() { set_flags(Flags::Promotion, ~0); };

Piece Move::moving_piece() const
{
    auto piece_type = static_cast<Piece::Type>(get_flags(Flags::PieceType));
    return Piece(piece_type, get_color());
}

Piece Move::resulting_piece() const
{
    // TODO: Handle promotions
    auto piece_type = static_cast<Piece::Type>(get_flags(Flags::PieceType));
    return Piece(piece_type, get_color());
}

uint32_t Move::get_flags(Flags flags) const
{
    return (m_data.to_ulong() & masks[static_cast<uint32_t>(flags)]) >> shifts[static_cast<uint32_t>(flags)];
}

Color Move::get_color() const { return get_flags(Flags::Color) == 1 ? Color::White : Color::Black; }

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
void Move::set_promotion(Piece::Type type) { set_flags(Flags::Promotion, static_cast<uint32_t>(type)); }

Location Move::start_location() const { return Location(get_flags(Flags::Origin)); }
Location Move::end_location() const { return Location(get_flags(Flags::Destination)); }

bool Move::is_capture() const { return get_flags(Flags::Capture) != 0; }
bool Move::is_promotion() const { return get_flags(Flags::Promotion) != 0; }
bool Move::is_en_passant() const { return get_flags(Flags::EnPassant) != 0; }
bool Move::is_double_pawn() const { return get_flags(Flags::DoublePawn) != 0; }

Piece::Type Move::captured_piece_type() const { return static_cast<Piece::Type>(get_flags(Flags::Capture)); }
Piece::Type Move::promoted_piece_type() const { return static_cast<Piece::Type>(get_flags(Flags::Promotion)); }

Move Move::by_moving(Piece piece, Location from, Location to)
{
    Move move;
    move.set_piece_type(piece.type());
    move.set_origin(from);
    move.set_destination(to);
    move.set_color(piece.color());
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

Move Move::by_capture_promoting(Piece piece, Location from, Location to, Piece::Type captured, Piece::Type promotion)
{
    Move move = by_moving(piece, from, to);
    move.set_capture(captured);
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

std::string Move::to_short_algebraic_notation() const
{
    std::string result;
    result += start_location().to_string();
    result += " -> ";
    result += end_location().to_string();
    result += " (";
    result += moving_piece().to_letter();
    result += ")";
    return result;
}

bool operator==(const Move& lhs, const Move& rhs) { return lhs.m_data == rhs.m_data; }

} // namespace weechess
