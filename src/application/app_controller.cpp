#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/string.hpp>

#include "app_controller.h"
#include "../board_utils.h"

AppController::AppController() {
    auto renderer = ftxui::Renderer([&](){
        return render();
    });

    // renderer |= ftxui::CatchEvent([&](ftxui::Event event) {
    //     return false;
    // });

    m_renderer = renderer;
}

const ftxui::Component& AppController::renderer() const {
    return m_renderer;
}

void AppController::set_delegate(std::weak_ptr<Delegate> delegate) {
    m_delegate = delegate;
}

void AppController::update_state(std::function<bool(State&)> fn) {
    bool is_dirty = fn(m_state);

    if (is_dirty) {
        if (auto delegate = m_delegate.lock()) {
            delegate->on_should_redraw();
        }
    }
}

static const std::string rank_labels =   " 1 2 3 4 5 6 7 8 ";
static const std::string file_labels =   "  A   B   C   D   E   F   G   H  ";

AppController::CommandInput::CommandInput() {
    m_input = ftxui::Input(&m_text, "");
}

const std::string& AppController::CommandInput::text() const {
    return m_text;
}

const ftxui::Component& AppController::CommandInput::renderer() const {
    return m_input;
}

ftxui::Element AppController::render() {
    using namespace ftxui;

    auto board_render = BoardRender::from(m_state.board);

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
            vbox({
                /* Command history */
            }) | flex,
            hbox({
                /* Command input */
                text("> ") | color(Color::GrayDark),
                // m_view_state.command_input.renderer()->Render() | flex,
                text("") | flex
            }),
        }) | flex | border
    });

    return document;
}