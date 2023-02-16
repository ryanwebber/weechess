#include <array>

#include <weechess/bit_board.h>
#include <weechess/move_generator.h>

namespace weechess {

constexpr std::array<BitBoard, 64> compute_knight_jumps()
{
    constexpr std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> knight_jumps = {
        std::make_tuple(Location::Left * 2, Location::Down),
        std::make_tuple(Location::Left * 2, Location::Up),
        std::make_tuple(Location::Right * 2, Location::Down),
        std::make_tuple(Location::Right * 2, Location::Up),
        std::make_tuple(Location::Left, Location::Down * 2),
        std::make_tuple(Location::Left, Location::Up * 2),
        std::make_tuple(Location::Right, Location::Down * 2),
        std::make_tuple(Location::Right, Location::Up * 2),
    };

    std::array<BitBoard, 64> results {};
    for (auto i = 0; i < results.size(); i++) {
        Location location(i);
        for (const auto& jump : knight_jumps) {
            auto [file_shift, rank_shift] = jump;
            if (auto new_location = location.offset_by(file_shift, rank_shift)) {
                results[i].set(*new_location);
            }
        }
    }

    return results;
}

constexpr std::array<BitBoard, 64> compute_king_jumps()
{
    constexpr std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> king_jumps = {
        std::make_tuple(Location::Left, Location::Down),
        std::make_tuple(Location::Left, Location::Up),
        std::make_tuple(Location::Right, Location::Down),
        std::make_tuple(Location::Right, Location::Up),
        std::make_tuple(Location::Left, Location::RankShift {}),
        std::make_tuple(Location::Right, Location::RankShift {}),
        std::make_tuple(Location::FileShift {}, Location::Down),
        std::make_tuple(Location::FileShift {}, Location::Up),
    };

    std::array<BitBoard, 64> results {};
    for (auto i = 0; i < results.size(); i++) {
        Location location(i);
        for (const auto& jump : king_jumps) {
            auto [file_shift, rank_shift] = jump;
            if (auto new_location = location.offset_by(file_shift, rank_shift)) {
                results[i].set(*new_location);
            }
        }
    }

    return results;
}

const std::array<BitBoard, 64> k_knight_jumps = compute_knight_jumps();
const std::array<BitBoard, 64> k_king_jumps = compute_king_jumps();

MoveGenerator::Request::Request(Color turn_to_move, ColorMap<CastleRights> castle_rights)
    : turn_to_move(turn_to_move)
    , castle_rights(castle_rights)
    , occupancy({})
    , en_passant_target({})
{
}

void MoveGenerator::Request::add_piece(const Piece& piece, const Location& location)
{
    occupancy[static_cast<uint8_t>(piece.type())][piece.color()].set(location);
}

void MoveGenerator::Request::set_en_passant_target(const Location& location) { en_passant_target = location; }

MoveGenerator::Result MoveGenerator::execute(const Request& request) const
{
    Result result {};
    return result;
}

} // namespace weechess
