#include <iostream>
#include <sstream>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

#include "game_state.h"

static const std::string rank_labels =   " 1 2 3 4 5 6 7 8 ";
static const std::string file_labels =   "  A   B   C   D   E   F   G   H  ";
constexpr std::string_view top_row =     "╭───┬───┬───┬───┬───┬───┬───┬───╮";
constexpr std::string_view mid_row =     "├───┼───┼───┼───┼───┼───┼───┼───┤";
constexpr std::string_view bottom_row =  "╰───┴───┴───┴───┴───┴───┴───┴───╯";
constexpr std::string_view bar = "│";

struct BoardRender {
    std::vector<std::string> lines;
    size_t width;
    size_t height;

    static BoardRender from(const weechess::Board& board) {
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
};

ftxui::Element render(const weechess::GameState &gamestate) {
    using namespace ftxui;

    auto board_render = BoardRender::from(gamestate.board());

    std::vector<Element> board_rows;
    for (auto line : board_render.lines) {
        board_rows.push_back(text(line));
    }

    auto format_move = [](weechess::Piece piece, weechess::Location location) {
        return piece.to_symbol() + " → " + location.to_string();
    };

    std::vector<std::vector<std::string>> history;
    constexpr size_t max_history = 16;
    for (auto i = 0; i < max_history; i++) {
        history.push_back({
            std::to_string(i),
            format_move(weechess::Piece(weechess::PieceType::Pawn, weechess::Color::White), weechess::Location::from_name("e3").value()),
            format_move(weechess::Piece(weechess::PieceType::Pawn, weechess::Color::Black), weechess::Location::from_name("d5").value()),
        });
    }

    auto table = Table(std::move(history));

    table.SelectColumn(0).Decorate(color(Color::GrayDark));
    table.SelectColumn(0).Decorate(center);
    table.SelectColumn(0).Decorate(flex);
    table.SelectColumn(1).Decorate(flex);
    table.SelectColumn(2).Decorate(flex);

    auto document = vbox({
        hbox({
            vtext(rank_labels) | hcenter | size(WIDTH, EQUAL, 3) | color(Color::GrayDark),
            vbox({
                vbox(std::move(board_rows)),
                text(file_labels) | hcenter | color(Color::GrayDark),
            }),
            vbox({})| size(WIDTH, EQUAL, 2),
            window(text("History"), table.Render() | size(WIDTH, EQUAL, 24)),
            window(text("Logs"), text("")) | flex,
        }),
        vbox({
        }) | flex | border
    });

    return document;
}

int main(int argc, char *argv[]) {
    auto gamestate = weechess::GameState::new_game();

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    screen.Loop(ftxui::Renderer([&](){
        return render(gamestate);
    }));
}
