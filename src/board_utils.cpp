#include "board_utils.h"

constexpr std::string_view top_row =     "╭───┬───┬───┬───┬───┬───┬───┬───╮";
constexpr std::string_view mid_row =     "├───┼───┼───┼───┼───┼───┼───┼───┤";
constexpr std::string_view bottom_row =  "╰───┴───┴───┴───┴───┴───┴───┴───╯";
constexpr std::string_view bar = "│";

BoardRender BoardRender::from(const weechess::Board& board) {
    std::vector<std::string> lines;

    lines.push_back(std::string(top_row));
    for (auto r = 0; r < 8; r++) {
        std::stringstream ss;
        ss << bar;
        for (auto f = 0; f < 8; f++) {
            weechess::Location location = weechess::Location::from_rank_and_file(r, f);
            weechess::Piece piece = board.piece_at(location);

            auto piece_symbol = piece.to_symbol();
            ss << " " << piece_symbol << " │";
        }

        lines.push_back(ss.str());
        if (r < 7) {
            lines.push_back(std::string(mid_row));
        }
    }

    lines.push_back(std::string(bottom_row));

    return BoardRender { lines, top_row.size(), top_row.size() };
}
