#include <sstream>
#include <weechess/bit_board.h>

namespace weechess {

std::string BitBoard::to_string() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const BitBoard& bb)
{
    for (auto r = 7; r >= 0; r--) {
        for (auto f = 0; f < 8; f++) {
            os << (bb[Location::from_rank_and_file(r, f)] ? '1' : '.');
        }

        if (r != 0) {
            os << std::endl;
        }
    }

    return os;
}
}
