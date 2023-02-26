#include <sstream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "../board_printer.h"
#include "../log.h"
#include "../string_utils.h"
#include "app_controller.h"

std::vector<std::string> split(std::string_view sv)
{
    std::stringstream ss((std::string(sv)));
    std::vector<std::string> out;
    std::string s;
    while (std::getline(ss, s)) {
        // Hack to get empty lines displaying correctly.
        // It sucks that we have to do this line by line
        // because the paragraph element doesn't support
        // newlines
        if (s.empty())
            s += " ";
        out.push_back(std::move(s));
    }

    return out;
}

void AppController::State::push_command_error(std::string_view text)
{
    for (const auto& line : split(text)) {
        command_output.push_back({ .text = line, .type = CommandOutput::Type::Error });
    }
}

void AppController::State::push_command_info(std::string_view text)
{
    for (const auto& line : split(text)) {
        command_output.push_back({ .text = line, .type = CommandOutput::Type::Info });
    }
}

AppController::AppController()
{
    auto renderer = ftxui::Renderer([&] { return render(); });

    renderer |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('?')) {
            if (auto delegate = m_delegate.lock())
                delegate->on_debug_event(*this);
            return true;
        }

        auto try_perform_move = [&](const weechess::Move& move) {
            auto possible_moves = m_state.game_state.move_set().legal_moves_from(move.start_location());
            if (std::find(possible_moves.begin(), possible_moves.end(), move) == possible_moves.end()) {

                // User selected a different piece of their own. We obviously can't move there, so
                // instead we select the new piece
                if (m_state.game_state.board().piece_at(move.end_location()).is(m_state.game_state.turn_to_move())) {
                    m_view_state.pinned_location = move.end_location();
                } else {
                    m_view_state.pinned_location = {};
                }

                return;
            }

            if (auto delegate = m_delegate.lock()) {
                auto move_cmd = "move " + move.start_location().to_string() + " " + move.end_location().to_string();
                m_state.command_output.push_back({ "> " + move_cmd, CommandOutput::Type::Command });
                delegate->on_execute_command(*this, move_cmd);
            }

            m_view_state.pinned_location = {};
        };

        // TODO: Remove this
        (void)try_perform_move;

        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Button::Left
            && event.mouse().motion == ftxui::Mouse::Motion::Released) {
            if (m_view_state.chess_window_bounds.Contain(event.mouse().x, event.mouse().y)) {
                auto xmin = m_view_state.chess_window_bounds.x_min;
                auto ymin = m_view_state.chess_window_bounds.y_min;
                auto xmax = m_view_state.chess_window_bounds.x_max;
                auto ymax = m_view_state.chess_window_bounds.y_max;
                auto width = xmax - xmin;
                auto height = ymax - ymin;

                int file = 8 * (event.mouse().x - xmin) / width;
                int rank = 8 * (event.mouse().y - ymin) / height;
                if (rank < 8 && file < 8) {
                    m_view_state.focus = ViewState::Focus::ChessWindow;
                    m_view_state.highlighted_location = weechess::Location::from_rank_and_file(rank, file);

                    if (m_view_state.pinned_location.has_value()) {
                        // TODO: Find this move and apply it
                        // auto from = m_view_state.pinned_location.value();
                        // auto to = m_view_state.highlighted_location;
                        assert(false);
                    } else {
                        m_view_state.pinned_location = m_view_state.highlighted_location;
                    }

                    return true;
                }
            }
        }

        // Pass any events to the current component in focus
        // if it can handle them
        auto current_focus = m_view_state.component_in_focus();
        if (current_focus != nullptr) {
            auto event_handled = current_focus->OnEvent(event);
            if (event_handled && event != ftxui::Event::Return)
                return true;
        }

        // Handle events that will switch focus between components
        if (event == ftxui::Event::Escape) {
            m_view_state.pinned_location = {};
            m_view_state.focus = ViewState::Focus::ChessWindow;
            return true;
        } else if (event == ftxui::Event::Character('/')) {
            m_view_state.pinned_location = {};
            m_view_state.focus = ViewState::Focus::CommandWindow;
            return true;
        }

        // Otherwise handle events per component
        switch (m_view_state.focus) {
        case ViewState::Focus::ChessWindow: {

            // Handle navigation around the board with arrows
            if (event == ftxui::Event::ArrowLeft) {
                if (auto l = m_view_state.highlighted_location.offset_by(weechess::Location::Left))
                    m_view_state.highlighted_location = l.value();
                return true;
            } else if (event == ftxui::Event::ArrowRight) {
                if (auto l = m_view_state.highlighted_location.offset_by(weechess::Location::Right))
                    m_view_state.highlighted_location = l.value();
                return true;
            } else if (event == ftxui::Event::ArrowUp) {
                if (auto l = m_view_state.highlighted_location.offset_by(weechess::Location::Up))
                    m_view_state.highlighted_location = l.value();
                return true;
            } else if (event == ftxui::Event::ArrowDown) {
                if (auto l = m_view_state.highlighted_location.offset_by(weechess::Location::Down))
                    m_view_state.highlighted_location = l.value();
                return true;
            }

            // Handle jumping to a specific file or rank on the board
            if (event.is_character() && event.character().size() > 0) {
                auto c = event.character()[0];
                if (c > '0' && c <= '8') {
                    m_view_state.highlighted_location
                        = weechess::Location::from_rank_and_file(c - '1', m_view_state.highlighted_location.file());
                    return true;
                } else if (c >= 'a' && c <= 'h') {
                    m_view_state.highlighted_location
                        = weechess::Location::from_rank_and_file(m_view_state.highlighted_location.rank(), c - 'a');
                    return true;
                }
            }

            // Handle pressing enter to pin the location
            if (event == ftxui::Event::Return) {
                if (m_view_state.pinned_location.has_value()) {
                    // TODO: Find this move and apply it
                    // auto from = m_view_state.pinned_location.value();
                    // auto to = m_view_state.highlighted_location;
                    assert(false);
                } else {
                    m_view_state.pinned_location = m_view_state.highlighted_location;
                }

                return true;
            }
        }
        case ViewState::Focus::CommandWindow: {
            if (event == ftxui::Event::Return) {
                auto& command_text = m_view_state.command_input.text();
                if (command_text.size() > 0) {
                    m_state.command_output.push_back({ "> " + command_text, CommandOutput::Type::Command });
                    if (auto delegate = m_delegate.lock()) {
                        delegate->on_execute_command(*this, command_text);
                    }

                    command_text.clear();
                }
            }
        }
        }

        return false;
    });

    m_renderer = renderer;
}

const ftxui::Component& AppController::renderer() const { return m_renderer; }

void AppController::set_delegate(std::weak_ptr<Delegate> delegate) { m_delegate = delegate; }

const AppController::State& AppController::state() const { return m_state; }

void AppController::update_state(std::function<bool(State&)> fn)
{
    bool is_dirty = fn(m_state);

    if (is_dirty) {
        if (auto delegate = m_delegate.lock()) {
            delegate->on_should_redraw(*this);
        }
    }
}

static const std::string rank_labels = " 1 2 3 4 5 6 7 8 ";
static const std::string file_labels = "  A   B   C   D   E   F   G   H  ";

AppController::CommandInput::CommandInput() { m_input = ftxui::Input(&m_text, ""); }

std::string& AppController::CommandInput::text() { return m_text; }

const std::string& AppController::CommandInput::text() const { return m_text; }

const ftxui::Component& AppController::CommandInput::renderer() const { return m_input; }

AppController::ViewState::ViewState() { focus = Focus::ChessWindow; }

ftxui::Component AppController::ViewState::component_in_focus() const
{
    switch (focus) {
    case Focus::ChessWindow:
        return nullptr;
    case Focus::CommandWindow:
        return command_input.renderer();
    }
}

enum class BoardDecoration {
    None = 0,
    Highlighted,
    Pinned,
    PossibleMove,
};

ftxui::Element AppController::render()
{
    using namespace ftxui;

    std::optional<weechess::Location> highlighted_location;
    if (m_view_state.focus == ViewState::Focus::ChessWindow) {
        highlighted_location = m_view_state.highlighted_location;
    }

    if (m_view_state.pinned_location.has_value()
        && !m_state.game_state.board()
                .piece_at(m_view_state.pinned_location.value())
                .is(m_state.game_state.turn_to_move())) {
        m_view_state.pinned_location = {};
    }

    BoardPrinter bp;
    BoardPrinter::Buffer<BoardDecoration> decorations = {};

    // Place the pieces on the board
    for (auto i = 0; i < weechess::Board::cell_count; i++) {
        weechess::Location l(i);
        auto piece = m_state.game_state.board().piece_at(l);
        if (piece.exists()) {
            bp[l].paint_symbol(piece.to_symbol());
        }
    }

    // Highlight the pinned cell
    if (m_view_state.pinned_location.has_value()) {
        auto cell = bp[m_view_state.pinned_location.value()];
        decorations[cell.offset()] = BoardDecoration::Pinned;
    }

    // Highlight the selected cell
    if (highlighted_location.has_value()) {
        auto cell = bp[highlighted_location.value()];
        cell.paint_border(BoardPrinter::Border::Detached);
        for (auto o : BoardPrinter::Cell::offsets)
            decorations[cell.offset() + o] = BoardDecoration::Highlighted;
    }

    auto move_to_show_hints
        = m_view_state.pinned_location.has_value() ? m_view_state.pinned_location : highlighted_location;

    // Highlight the possible moves
    if (move_to_show_hints.has_value()) {
        auto moves = m_state.game_state.move_set().legal_moves_from(move_to_show_hints.value());
        for (auto& m : moves) {
            auto cell = bp[m.end_location()];
            if (*cell == u' ')
                cell.paint_symbol(u'•');
            decorations[cell.offset()] = BoardDecoration::PossibleMove;
        }
    }

    std::vector<Element> board_rows;
    for (auto r = 0; r < bp.rows(); r++) {
        std::vector<Element> row_cells;
        for (auto c = 0; c < bp.cols(); c++) {
            auto cell = bp.cell_at(r, c);
            auto elem = text(to_string(*cell));

            switch (decorations[cell.offset()]) {
            case BoardDecoration::None:
                break;
            case BoardDecoration::Highlighted:
                if (m_view_state.pinned_location.has_value()) {
                    elem |= color(Color::Magenta);
                } else {
                    elem |= color(Color::Yellow);
                }

                break;
            case BoardDecoration::Pinned:
                elem |= color(Color::Magenta);
                break;
            case BoardDecoration::PossibleMove:
                elem |= color(Color::Cyan);
                break;
            }

            row_cells.push_back(elem);
        }

        board_rows.push_back(hbox(std::move(row_cells)));
    }

    auto format_move = [](weechess::Piece piece, weechess::Location location) {
        return to_string(piece.to_symbol()) + " → " + location.to_string();
    };

    std::vector<std::vector<std::string>> history {};

    {
        constexpr size_t max_history = 16;
        size_t i_start = m_state.move_history.size() > max_history ? m_state.move_history.size() - max_history : 0;

        // Offset to even numbers properly
        i_start = ((i_start + 1) / 2) * 2;

        for (auto i = i_start; i < i_start + max_history; i += 2) {
            auto s1 = i < m_state.move_history.size()
                ? format_move(m_state.move_history[i].piece, m_state.move_history[i].location)
                : "      ";

            auto s2 = (i + 1) < m_state.move_history.size()
                ? format_move(m_state.move_history[i + 1].piece, m_state.move_history[i + 1].location)
                : "      ";

            history.push_back({
                std::to_string(i / 2 + 1),
                s1,
                s2,
            });
        }
    }

    auto table = Table(std::move(history));

    table.SelectColumn(0).Decorate(dim);
    table.SelectColumn(0).Decorate(hcenter);
    table.SelectColumn(0).Decorate(flex);
    table.SelectColumn(1).Decorate(flex);
    table.SelectColumn(2).Decorate(flex);

    auto command_input = m_view_state.command_input.renderer()->Render();
    if (m_view_state.command_input.text().size() == 0) {
        command_input |= inverted;
    }

    auto prompt_decoration
        = m_view_state.focus == AppController::ViewState::Focus::CommandWindow ? color(Color::Yellow) : dim;

    std::vector<Element> command_history;
    for (const auto& output : m_state.command_output) {
        auto p = paragraph(output.text);
        switch (output.type) {
        case CommandOutput::Type::Info:
            p |= dim;
            break;
        case CommandOutput::Type::Error:
            p |= color(Color::Red);
            break;
        default:
            break;
        }

        command_history.push_back(p);
    }

    // clang-format off
    auto document = vbox({
        hbox({
            vtext(rank_labels) | hcenter | size(WIDTH, EQUAL, 3) | dim,
            vbox({
                vbox(std::move(board_rows)) | reflect(m_view_state.chess_window_bounds),
                text(file_labels) | hcenter | dim,
            }),
            filler() | size(WIDTH, EQUAL, 2),
            window(text("History") | hcenter, table.Render() | size(WIDTH, EQUAL, 23)),
            window(text("Logs") | hcenter, text("")) | flex,
        }),
        vbox({
            vbox({ command_history }) | focusPositionRelative(0, 1) | yframe | flex,
            hbox({
                text("> ") | prompt_decoration,
                command_input | flex,
            }),
        }) | flex | border
    });
    // clang-format on

    return document;
}
