#include <weechess/board.h>
#include <weechess/move_query.h>

#include "log.h"

namespace weechess {

LocationMoveQuery::LocationMoveQuery(Location from, Location to)
    : m_from(from)
    , m_to(to)
{
}

LocationMoveQuery::LocationMoveQuery(std::optional<Location> from, std::optional<Location> to)
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

PGNMoveQuery::PGNMoveQuery() = default;

bool PGNMoveQuery::test(const Move& move) const
{
    if (m_castle_side.has_value()) {
        return move.is_castle() && move.castle_side().value() == *m_castle_side;
    }

    if (m_destination.has_value() && move.end_location() != *m_destination) {
        return false;
    }

    if (m_origin_file.has_value() && move.start_location().file() != (7 - m_origin_file->index)) {
        return false;
    }

    if (m_origin_rank.has_value() && move.start_location().rank() != m_origin_rank->index) {
        // log::debug("Rank mismatch: {} != {}", move.start_location().rank(), m_origin_rank->index);
        return false;
    }

    if (m_piece_type.has_value() && move.moving_piece().type != *m_piece_type) {
        return false;
    }

    if (m_capture.has_value() && move.is_capture() != *m_capture) {
        return false;
    }

    if (m_promotion_type.has_value() && move.promoted_piece_type() != *m_promotion_type) {
        return false;
    }

    return true;
}

bool PGNMoveQuery::is_castle(CastleSide side) const { return m_castle_side.has_value() && *m_castle_side == side; }
bool PGNMoveQuery::is_capture() const { return m_capture.value_or(false); }
std::optional<Piece::Type> PGNMoveQuery::promotion_type() const { return m_promotion_type; }
std::optional<Piece::Type> PGNMoveQuery::piece_type() const { return m_piece_type; }
std::optional<Location> PGNMoveQuery::destination() const { return m_destination; }
std::optional<File> PGNMoveQuery::origin_file() const { return m_origin_file; }
std::optional<Rank> PGNMoveQuery::origin_rank() const { return m_origin_rank; }

std::optional<PGNMoveQuery> PGNMoveQuery::from(std::string_view notation)
{
    if (notation.size() < 2) {
        return {};
    }

    PGNMoveQuery query {};
    if (notation == "O-O") {
        query.m_castle_side = CastleSide::Kingside;
        return query;
    } else if (notation == "O-O-O") {
        query.m_castle_side = CastleSide::Queenside;
        return query;
    }

    // Work our way backwards through the notation
    auto itr = notation.end() - 1;

    if (*itr == '+' || *itr == '#') {
        itr--;
    }

    // Promotion
    if (*itr >= 'A' && *itr <= 'Z') {
        switch (*itr) {
        case 'Q':
            query.m_promotion_type = Piece::Type::Queen;
            break;
        case 'R':
            query.m_promotion_type = Piece::Type::Rook;
            break;
        case 'B':
            query.m_promotion_type = Piece::Type::Bishop;
            break;
        case 'N':
            query.m_promotion_type = Piece::Type::Knight;
            break;
        default:
            return {};
        }

        itr--;

        if (*itr == '=') {
            itr--;
        }
    }

    // Destination
    if (std::distance(notation.begin(), itr) < 1) {
        return {};
    }

    query.m_destination = Location::from_string(std::string_view(itr - 1, 2));
    itr -= 2;

    // Captures
    if (itr >= notation.begin() && *itr == 'x') {
        query.m_capture = true;
        itr--;
    }

    // Origin rank
    if (itr >= notation.begin() && *itr >= '1' && *itr <= '8') {
        query.m_origin_rank = Rank(*itr - '0');
        itr--;
    }

    // Origin file
    if (itr >= notation.begin() && *itr >= 'a' && *itr <= 'h') {
        query.m_origin_file = File(*itr);
        itr--;
    }

    // Origin piece type
    if (itr >= notation.begin() && *itr >= 'A' && *itr <= 'Z') {
        switch (*itr) {
        case 'K':
            query.m_piece_type = Piece::Type::King;
            break;
        case 'Q':
            query.m_piece_type = Piece::Type::Queen;
            break;
        case 'R':
            query.m_piece_type = Piece::Type::Rook;
            break;
        case 'B':
            query.m_piece_type = Piece::Type::Bishop;
            break;
        case 'N':
            query.m_piece_type = Piece::Type::Knight;
            break;
        case 'P':
            query.m_piece_type = Piece::Type::Pawn;
            break;
        default:
            return {};
        }

        itr--;
    }

    // At this point we should have no more characters left
    if (itr >= notation.begin()) {
        return {};
    }

    if (!query.m_piece_type.has_value())
        query.m_piece_type = Piece::Type::Pawn;

    if (!query.m_capture.has_value())
        query.m_capture = false;

    return query;
}

}
