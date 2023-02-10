#include <iostream>
#include <memory>
#include <sstream>

#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>

#include <weechess/game_state.h>

#include "application/app_controller.h"
#include "console.h"

class AppDelegate:
    public AppController::Delegate,
    public Console::Display,
    public Console::Service,
    public std::enable_shared_from_this<AppDelegate>
{
    private:
        Console m_console;
        AppController& m_controller;
        std::function<void()> m_exit_closure;

        AppDelegate(AppController &controller, std::function<void()> exit_closure)
            : m_controller(controller)
            , m_exit_closure(exit_closure) {}

    public:
        AppDelegate() = delete;
        AppDelegate(AppDelegate&&) = delete;
        AppDelegate(const AppDelegate&) = delete;

        void on_should_redraw(AppController&) override {
            // This will eventuall be necessary to redraw the screen
            // when we have multiple threads updating view state
        }

        void on_execute_command(AppController&, std::string_view command) override {
            m_console.execute(command);
        }

        void clear() override {
            m_controller.update_state([&](AppController::State &state) {
                state.command_output.clear();
                return true;
            });
        }

        void exit() override {
            m_exit_closure();
        }

        void write_stdout(std::string str) override {
            m_controller.update_state([&](AppController::State &state) {
                state.push_command_info(str);
                return true;
            });
        }

        void write_stderr(std::string str) override {
            m_controller.update_state([&](AppController::State &state) {
                state.push_command_error(str);
                return true;
            });
        }

        bool cmd_move_piece(weechess::Move move) override {
            return false;
        }

        static std::shared_ptr<AppDelegate> make_shared(AppController &controller, std::function<void()> exit_closure) {
            auto shared = std::shared_ptr<AppDelegate>(new AppDelegate(controller, exit_closure));
            shared->m_console.set_display(shared);
            shared->m_console.set_service(shared);

            return shared;
        }

        ~AppDelegate() = default;
};

int main(int argc, char *argv[]) {

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    AppController controller;
    auto delegate = AppDelegate::make_shared(controller, screen.ExitLoopClosure());
    controller.set_delegate(delegate);

    // Bootstrap a new game here
    auto gamestate = weechess::GameState::new_game();
    controller.update_state([&](auto &state) {
        state.game_state = gamestate;
        return true;
    });

    screen.Loop(controller.renderer());
}
