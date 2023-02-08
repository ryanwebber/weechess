#include <iostream>
#include <memory>
#include <sstream>

#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>

#include "application/app_controller.h"
#include "arg_parser.h"
#include "game_state.h"

class AppDelegate: public AppController::Delegate {
    private:
        struct Command {
            std::string command_name;
            union {
                struct {
                    bool show_help;
                } move;
            } command;
        };

        argparse::Parser<Command> m_cmd_parser;

        std::vector<std::string> tokenize(std::string_view &sv) {
            std::stringstream ss((std::string(sv)));
            std::vector<std::string> out;
            std::string s;
            while (std::getline(ss, s, ' ')) {
                out.push_back(s);
            }

            return out;
        }

    public:
        AppDelegate() : m_cmd_parser("In-game interactive console") {
            m_cmd_parser.add_command("help", "Show help and exit", [&](auto &parser) {
            });

            m_cmd_parser.add_command("move", "Move a piece", [&](auto &parser) {
                parser.add_option({
                    { "--help", "-h" },
                    "Show this help message",
                    [](auto &command, auto &args) {
                        args = args.subspan(1);
                        return true;
                    }
                });

                parser.set_argument_placeholder({
                    argparse::Cardinality::Multiple,
                    { "from", "to" },
                    [](auto &command, auto &args) {
                        if (args.size() < 2)
                            return false;

                        args = args.subspan(1);
                        return true;
                    }
                });
            });
        }

        void on_should_redraw(AppController&) override {

        }

        void on_execute_command(AppController &controller, std::string_view command) override {
            Command c;
            auto args = tokenize(command);

            auto print_usage = [&](AppController::State &state) {
                state.command_output.push_back({
                    "Usage:",
                    AppController::CommandOutput::Type::Info
                });

                for (const auto &usage : m_cmd_parser.example_usages()) {
                    state.command_output.push_back({
                        "  " + usage,
                        AppController::CommandOutput::Type::Info
                    });
                }
            };

            controller.update_state([&](auto &state) {
                std::span<std::string> parse_span = args;
                if (!m_cmd_parser.parse(c, parse_span)) {
                    state.command_output.push_back({
                        std::string("Unexpected command: ") + std::string(command),
                        AppController::CommandOutput::Type::Error
                    });

                    print_usage(state);
                } else {
                    state.command_output.push_back({
                        "Ok",
                        AppController::CommandOutput::Type::Info
                    });
                }

                return true;
            });
        }
};

int main(int argc, char *argv[]) {

    auto delegate = std::make_shared<AppDelegate>();

    AppController controller;
    controller.set_delegate(delegate);

    auto gamestate = weechess::GameState::new_game();

    controller.update_state([&](auto &state) {
        state.game_state = gamestate;
        return true;
    });

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    screen.Loop(controller.renderer());
}
