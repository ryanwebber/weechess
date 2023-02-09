#include "console.h"

std::vector<std::string> tokenize(std::string_view str) {
    std::vector<std::string> tokens;
    std::string token;
    for (auto c : str) {
        if (c == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token.push_back(c);
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

Console::Console(std::weak_ptr<Display> display, std::weak_ptr<Service> service)
    : m_display(display)
    , m_service(service)
{
}

void Console::set_display(std::weak_ptr<Display> display) {
    m_display = display;
}

void Console::set_service(std::weak_ptr<Service> service) {
    m_service = service;
}

void Console::execute(std::string_view command) {
    if (m_commands.empty()) {
        initialize();
    }

    auto display = m_display.lock();
    auto service = m_service.lock();
    if (!display || !service) {
        return;
    }

    auto tokens = tokenize(command);
    if (tokens.empty()) {
        return;
    }

    argparse::ArgumentParser *program = nullptr;
    for (auto &cmd : m_commands) {
        for (auto &name : cmd.names) {
            if (name == tokens[0]) {
                program = &cmd.parser;
                break;
            }
        }
    }

    if (program == nullptr) {
        display->write_stderr("Unknown command: " + tokens[0]);
        print_usage();
        return;
    }

    display->write_stdout("Executing command: " + tokens[0]);

    try {
        program->parse_args(tokens);
    } catch (const std::runtime_error& err) {
        display->write_stderr(err.what());
        display->write_stdout(program->usage());
        return;
    }


}

void Console::print_usage() const {
    constexpr size_t column_size = 12;
    if (auto display = m_display.lock()) {
        display->write_stdout("Commands:");
        for (auto &cmd : m_commands) {
            std::string command_listing;
            if (!cmd.names.empty()) {
                command_listing += cmd.names[0];

                for (size_t i = 1; i < cmd.names.size(); ++i) {
                    command_listing += ", " + cmd.names[i];
                }
            }

            if (command_listing.size() < column_size) {
                command_listing += std::string(column_size - command_listing.size(), ' ');
            }

            command_listing += cmd.parser.description();

            display->write_stdout("    " + command_listing);
        }
    }
}

void Console::initialize() {
    if (!m_commands.empty()) {
        return;
    }

    // Help command
    argparse::ArgumentParser cmd_help_parser("help", "1.0", argparse::default_arguments::none);
    cmd_help_parser.add_description("Print the available commands");
    m_commands.push_back({ { "help" }, cmd_help_parser });

    // Exit command
    argparse::ArgumentParser cmd_exit_parser("exit", "1.0", argparse::default_arguments::none);
    cmd_exit_parser.add_description("Exit the game");
    cmd_exit_parser.add_argument("--help")
        .default_value(false)
        .implicit_value(true);
    m_commands.push_back({ { "exit" }, cmd_exit_parser });

    // Move command
    argparse::ArgumentParser cmd_move_parser("move", "1.0", argparse::default_arguments::none);
    cmd_move_parser.add_argument("--help")
        .default_value(false)
        .implicit_value(true);

    cmd_move_parser.add_description("Make a move on the board");
    m_commands.push_back({ { "move", "m" }, cmd_move_parser });
}
