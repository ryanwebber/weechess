#include <weechess/board.h>
#include <weechess/location.h>

namespace weechess {

bool Location::is_valid() const { return offset < Board::cell_count; }

std::string Location::to_string() const { return std::string(1, 'A' + file()) + std::to_string(rank() + 1); }

std::optional<Location> Location::from_string(std::string_view name)
{
    if (name.size() != 2) {
        return std::nullopt;
    }

    uint8_t file = name[0] - 'a';
    uint8_t rank = name[1] - '1';

    if (file > 7 || rank > 7) {
        return std::nullopt;
    }

    return Location::from_rank_and_file(rank, file);
}

const Location Location::A1 = Location::from_rank_and_file(0, 0);
const Location Location::B1 = Location::from_rank_and_file(0, 1);
const Location Location::C1 = Location::from_rank_and_file(0, 2);
const Location Location::D1 = Location::from_rank_and_file(0, 3);
const Location Location::E1 = Location::from_rank_and_file(0, 4);
const Location Location::F1 = Location::from_rank_and_file(0, 5);
const Location Location::G1 = Location::from_rank_and_file(0, 6);
const Location Location::H1 = Location::from_rank_and_file(0, 7);
const Location Location::A2 = Location::from_rank_and_file(1, 0);
const Location Location::B2 = Location::from_rank_and_file(1, 1);
const Location Location::C2 = Location::from_rank_and_file(1, 2);
const Location Location::D2 = Location::from_rank_and_file(1, 3);
const Location Location::E2 = Location::from_rank_and_file(1, 4);
const Location Location::F2 = Location::from_rank_and_file(1, 5);
const Location Location::G2 = Location::from_rank_and_file(1, 6);
const Location Location::H2 = Location::from_rank_and_file(1, 7);
const Location Location::A3 = Location::from_rank_and_file(2, 0);
const Location Location::B3 = Location::from_rank_and_file(2, 1);
const Location Location::C3 = Location::from_rank_and_file(2, 2);
const Location Location::D3 = Location::from_rank_and_file(2, 3);
const Location Location::E3 = Location::from_rank_and_file(2, 4);
const Location Location::F3 = Location::from_rank_and_file(2, 5);
const Location Location::G3 = Location::from_rank_and_file(2, 6);
const Location Location::H3 = Location::from_rank_and_file(2, 7);
const Location Location::A4 = Location::from_rank_and_file(3, 0);
const Location Location::B4 = Location::from_rank_and_file(3, 1);
const Location Location::C4 = Location::from_rank_and_file(3, 2);
const Location Location::D4 = Location::from_rank_and_file(3, 3);
const Location Location::E4 = Location::from_rank_and_file(3, 4);
const Location Location::F4 = Location::from_rank_and_file(3, 5);
const Location Location::G4 = Location::from_rank_and_file(3, 6);
const Location Location::H4 = Location::from_rank_and_file(3, 7);
const Location Location::A5 = Location::from_rank_and_file(4, 0);
const Location Location::B5 = Location::from_rank_and_file(4, 1);
const Location Location::C5 = Location::from_rank_and_file(4, 2);
const Location Location::D5 = Location::from_rank_and_file(4, 3);
const Location Location::E5 = Location::from_rank_and_file(4, 4);
const Location Location::F5 = Location::from_rank_and_file(4, 5);
const Location Location::G5 = Location::from_rank_and_file(4, 6);
const Location Location::H5 = Location::from_rank_and_file(4, 7);
const Location Location::A6 = Location::from_rank_and_file(5, 0);
const Location Location::B6 = Location::from_rank_and_file(5, 1);
const Location Location::C6 = Location::from_rank_and_file(5, 2);
const Location Location::D6 = Location::from_rank_and_file(5, 3);
const Location Location::E6 = Location::from_rank_and_file(5, 4);
const Location Location::F6 = Location::from_rank_and_file(5, 5);
const Location Location::G6 = Location::from_rank_and_file(5, 6);
const Location Location::H6 = Location::from_rank_and_file(5, 7);
const Location Location::A7 = Location::from_rank_and_file(6, 0);
const Location Location::B7 = Location::from_rank_and_file(6, 1);
const Location Location::C7 = Location::from_rank_and_file(6, 2);
const Location Location::D7 = Location::from_rank_and_file(6, 3);
const Location Location::E7 = Location::from_rank_and_file(6, 4);
const Location Location::F7 = Location::from_rank_and_file(6, 5);
const Location Location::G7 = Location::from_rank_and_file(6, 6);
const Location Location::H7 = Location::from_rank_and_file(6, 7);
const Location Location::A8 = Location::from_rank_and_file(7, 0);
const Location Location::B8 = Location::from_rank_and_file(7, 1);
const Location Location::C8 = Location::from_rank_and_file(7, 2);
const Location Location::D8 = Location::from_rank_and_file(7, 3);
const Location Location::E8 = Location::from_rank_and_file(7, 4);
const Location Location::F8 = Location::from_rank_and_file(7, 5);
const Location Location::G8 = Location::from_rank_and_file(7, 6);
const Location Location::H8 = Location::from_rank_and_file(7, 7);

std::ostream& operator<<(std::ostream& os, const Location& location)
{
    os << location.to_string();
    return os;
}

}
