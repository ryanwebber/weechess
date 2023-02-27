#pragma once

#include <optional>

#include <weechess/location.h>
#include <weechess/move.h>
#include <weechess/piece.h>

namespace weechess {

class MoveQuery {
public:
    MoveQuery() = default;
    virtual ~MoveQuery() = default;

    virtual bool test(const Move& move) const = 0;
};

class LocationMoveQuery : public MoveQuery {
public:
    LocationMoveQuery(Location from, std::optional<Location> to = {});
    bool test(const Move& move) const override;

private:
    Location m_from;
    std::optional<Location> m_to;
};

class CastleMoveQuery : public MoveQuery {
public:
    CastleMoveQuery(CastleSide side);
    bool test(const Move& move) const override;

private:
    CastleSide m_side;
};

}
