#include <iostream>
#include <memory>
#include <sstream>

#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>

#include <weechess/game_state.h>

#include "application/app_controller.h"
#include "console.h"
#include "log.h"
#include "string_utils.h"

class AppDelegate : public AppController::Delegate,
                    public Console::Display,
                    public Console::Service,
                    public std::enable_shared_from_this<AppDelegate> {
private:
    Console m_console;
    AppController& m_controller;
    std::function<void()> m_exit_closure;

    AppDelegate(AppController& controller, std::function<void()> exit_closure)
        : m_controller(controller)
        , m_exit_closure(exit_closure)
    {
    }

public:
    AppDelegate() = delete;
    AppDelegate(AppDelegate&&) = delete;
    AppDelegate(const AppDelegate&) = delete;

    void on_should_redraw(AppController&) override
    {
        // This will eventuall be necessary to redraw the screen
        // when we have multiple threads updating view state
    }

    void on_execute_command(AppController&, std::string_view command) override { m_console.execute(command); }

    void on_debug_event(AppController& controller) override { }

    void clear() override
    {
        m_controller.update_state([&](AppController::State& state) {
            state.command_output.clear();
            return true;
        });
    }

    void exit() override { m_exit_closure(); }

    void write_stdout(std::string str) override
    {
        m_controller.update_state([&](AppController::State& state) {
            state.push_command_info(str);
            return true;
        });
    }

    void write_stderr(std::string str) override
    {
        m_controller.update_state([&](AppController::State& state) {
            state.push_command_error(str);
            return true;
        });
    }

    bool cmd_perform_move(const weechess::MoveQuery& query) override
    {
        auto possible_moves = m_controller.state().game_state.move_set().find(query);
        if (possible_moves.size() == 0) {
            write_stderr("Illegal move");
            return false;
        } else if (possible_moves.size() > 1) {
            write_stderr("Ambiguous move");
            return false;
        }

        auto move = possible_moves[0];
        m_controller.update_state([&](AppController::State& state) {
            state.move_history.push_back({
                move,
                state.game_state.san_notation(move),
            });

            state.game_state = weechess::GameState::by_performing_move(state.game_state, move).value();
            return true;
        });

        return true;
    }

    static std::shared_ptr<AppDelegate> make_shared(AppController& controller, std::function<void()> exit_closure)
    {
        auto shared = std::shared_ptr<AppDelegate>(new AppDelegate(controller, exit_closure));
        shared->m_console.set_display(shared);
        shared->m_console.set_service(shared);

        return shared;
    }

    ~AppDelegate() = default;
};

uint64_t do_perft(weechess::GameState gs, int depth, bool print = false)
{
    if (depth == 0) {
        return 1;
    } else if (depth == 1) {
        return gs.move_set().legal_moves().size();
    }

    uint64_t count = 0;
    for (const auto& move : gs.move_set().legal_moves()) {
        auto new_gs = weechess::GameState::by_performing_move(gs, move).value();
        auto result = do_perft(std::move(new_gs), depth - 1);
        count += result;

        // if (print) {
        //     UNSCOPED_INFO(gs.san_notation(move) << ": " << result << "\n");
        // }
    }

    return count;
}

int main(int argc, char* argv[])
{
    // TODO: parse agruments
    if (argc > 1) {
        return 0;
    }

    auto gamestate = weechess::GameState::new_game();

    log::init_logging();

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    AppController controller;
    auto delegate = AppDelegate::make_shared(controller, screen.ExitLoopClosure());
    controller.set_delegate(delegate);

    controller.update_state([&](auto& state) {
        state.game_state = gamestate;
        return true;
    });

    screen.Loop(controller.renderer());
}
