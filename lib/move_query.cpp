#include <weechess/move_query.h>

namespace weechess {

LocationMoveQuery::LocationMoveQuery(Location from, std::optional<Location> to)
    : m_from(from)
    , m_to(to)
{
}

bool LocationMoveQuery::test(const Move& move) const
{
    return move.start_location() == m_from && (!m_to.has_value() || move.end_location() == m_to);
}

CastleMoveQuery::CastleMoveQuery(CastleSide side)
    : m_side(side)
{
}

bool CastleMoveQuery::test(const Move& move) const { return move.castle_side() == m_side; }

}
