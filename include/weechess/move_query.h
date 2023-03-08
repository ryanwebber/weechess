#pragma once

#include <memory>
#include <optional>
#include <string_view>

#include <weechess/board.h>
#include <weechess/location.h>
#include <weechess/move.h>
#include <weechess/piece.h>

namespace weechess {

class MoveQuery {
public:
    virtual ~MoveQuery() = default;
    virtual bool test(const Move& move) const = 0;
};

class LocationMoveQuery : public MoveQuery {
public:
    LocationMoveQuery(Location from, Location to);
    LocationMoveQuery(std::optional<Location> from, std::optional<Location> to);

    bool test(const Move& move) const override;

    static LocationMoveQuery from(Location from) { return { from, {} }; }
    static LocationMoveQuery to(Location to) { return { {}, to }; }

private:
    std::optional<Location> m_from;
    std::optional<Location> m_to;
};

class CastleMoveQuery : public MoveQuery {
public:
    CastleMoveQuery(CastleSide side);
    bool test(const Move& move) const override;

private:
    CastleSide m_side;
};

class PGNMoveQuery : public MoveQuery {
public:
    PGNMoveQuery();

    bool test(const Move& move) const override;

    bool is_castle(CastleSide) const;
    bool is_capture() const;
    std::optional<Piece::Type> promotion_type() const;
    std::optional<Piece::Type> piece_type() const;
    std::optional<Location> destination() const;
    std::optional<File> origin_file() const;
    std::optional<Rank> origin_rank() const;

    static std::optional<PGNMoveQuery> from(std::string_view notation);

private:
    std::optional<CastleSide> m_castle_side {};
    std::optional<File> m_origin_file {};
    std::optional<Rank> m_origin_rank {};
    std::optional<Location> m_destination {};
    std::optional<Piece::Type> m_promotion_type {};
    std::optional<Piece::Type> m_piece_type {};
    std::optional<bool> m_capture {};
};

}
