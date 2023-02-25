#include <weechess/move.h>

namespace weechess {

uint32_t Move::get_flags(Flags flags) const
{
    return (m_data.to_ulong() & masks[static_cast<uint32_t>(flags)]) >> shifts[static_cast<uint32_t>(flags)];
}

void Move::set_flags(Flags flags, uint32_t value)
{
    m_data &= ~masks[static_cast<uint32_t>(flags)];
    m_data |= (value << shifts[static_cast<uint32_t>(flags)]);
}

void Move::set_piece_type(Piece::Type type) { set_flags(Flags::PieceType, static_cast<uint32_t>(type)); }
void Move::set_origin(Location location) { set_flags(Flags::Origin, location.offset); }
void Move::set_destination(Location location) { set_flags(Flags::Destination, location.offset); }

Location Move::start_location() const { return Location(get_flags(Flags::Origin)); }
Location Move::end_location() const { return Location(get_flags(Flags::Destination)); }

Move Move::by_moving(Piece piece, Location from, Location to)
{
    Move move;
    move.set_piece_type(piece.type());
    move.set_origin(from);
    move.set_destination(to);
    return move;
}

bool operator==(const Move& lhs, const Move& rhs) { return lhs.m_data == rhs.m_data; }

} // namespace weechess
