#include <iostream>
#include <memory>
#include <sstream>

#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>

#include <argparse/argparse.h>
#include <weechess/game_state.h>

#include "app_controller.h"
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

        auto legal_move = possible_moves[0];
        m_controller.update_state([&](AppController::State& state) {
            state.move_history.push_back({
                legal_move.move(),
                legal_move->san_notation(state.game_state),
            });

            state.game_state = weechess::GameState(legal_move.snapshot());
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

int main(int argc, char* argv[])
{
    argparse::ArgumentParser parser("play", WEECHESS_PROJECT_VERSION, argparse::default_arguments::none);
    parser.add_description("Play an interactive game in the terminal.");
    parser.add_argument("--help").default_value(false).implicit_value(true);
    parser.add_argument("--fen").metavar("FEN").help("The initial position of the board in FEN notation");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    if (parser.get<bool>("--help")) {
        std::cout << parser;
        std::exit(0);
    }

    auto gamestate = weechess::GameState::new_game();
    if (auto fen = parser.present<std::string>("--fen")) {
        if (auto maybe_gamestate = weechess::GameState::from_fen(*fen)) {
            gamestate = *maybe_gamestate;
        } else {
            std::cerr << "Invalid FEN: " << *fen << std::endl;
            std::exit(1);
        }
    }

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
