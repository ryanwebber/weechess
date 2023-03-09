#include "console.h"
#include "log.h"

void with_help(argparse::ArgumentParser& parser)
{
    parser.add_argument("--help").default_value(false).implicit_value(true);
}

class ClearCommand : public Console::Command {
public:
    ClearCommand()
        : Command("clear", {}, [](auto& parser) {
            with_help(parser);
            parser.add_description("Clear the console");
        })
    {
    }

    void execute(const Console& console, argparse::ArgumentParser) const override
    {
        if (auto display = console.display().lock()) {
            display->clear();
        }
    }
};

class ExitCommand : public Console::Command {
public:
    ExitCommand()
        : Command("exit", {}, [](auto& parser) {
            with_help(parser);
            parser.add_description("Exit the application");
        })
    {
    }

    void execute(const Console& console, argparse::ArgumentParser) const override
    {
        if (auto display = console.display().lock()) {
            display->exit();
        }
    }
};

class HelpCommand : public Console::Command {
public:
    HelpCommand()
        : Command("help", {}, [](auto& parser) { parser.add_description("Print the available commands"); })
    {
    }

    void execute(const Console& console, argparse::ArgumentParser) const override { console.print_usage(); }
};

class MoveCommand : public Console::Command {
public:
    MoveCommand()
        : Command("move", {}, [](auto& parser) {
            with_help(parser);
            parser.add_description("Make a move on the board");
            parser.add_argument("from").help("The piece to move, by its location in standard notation");
            parser.add_argument("to").help("The location to move to, in standard notation");
        })
    {
    }

    void execute(const Console& console, argparse::ArgumentParser parser) const override
    {
        if (auto service = console.service().lock()) {
            auto origin_sn = parser.get<std::string>("from");
            auto destination_sn = parser.get<std::string>("to");

            auto origin = weechess::Location::from_string(origin_sn);
            auto destination = weechess::Location::from_string(destination_sn);

            if (!origin) {
                if (auto display = console.display().lock()) {
                    display->write_stderr("Invalid location: " + origin_sn);
                }
                return;
            }

            if (!destination) {
                if (auto display = console.display().lock()) {
                    display->write_stderr("Invalid location: " + destination_sn);
                }
                return;
            }

            auto move_query = weechess::LocationMoveQuery(*origin, *destination);
            service->cmd_perform_move(move_query);
        }
    }
};

std::vector<std::string> tokenize(std::string_view str)
{
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

Console::Console()
{
    m_commands.push_back(std::make_unique<ClearCommand>());
    m_commands.push_back(std::make_unique<ExitCommand>());
    m_commands.push_back(std::make_unique<HelpCommand>());
    m_commands.push_back(std::make_unique<MoveCommand>());
}

std::weak_ptr<Console::Display> Console::display() const { return m_display; }

void Console::set_display(std::weak_ptr<Display> display) { m_display = display; }

std::weak_ptr<Console::Service> Console::service() const { return m_service; }

void Console::set_service(std::weak_ptr<Service> service) { m_service = service; }

void Console::execute(std::string_view command)
{
    auto display = m_display.lock();
    auto service = m_service.lock();
    if (!display || !service) {
        return;
    }

    logger::debug("executing console command: {}", command);

    auto tokens = tokenize(command);
    if (tokens.empty()) {
        return;
    }

    auto command_itr = std::find_if(m_commands.begin(), m_commands.end(), [&](auto& cmd) {
        return std::find(cmd->names().begin(), cmd->names().end(), tokens[0]) != cmd->names().end();
    });

    if (command_itr == m_commands.end()) {
        display->write_stderr("Unknown command: " + tokens[0]);
        print_usage();
        return;
    }

    (*command_itr)->execute(*this, tokens);
}

void Console::print_usage() const
{
    constexpr size_t column_size = 12;
    if (auto display = m_display.lock()) {
        display->write_stdout("Commands:");
        for (auto& cmd : m_commands) {
            std::string command_listing;
            auto names = cmd->names();
            if (!names.empty()) {
                command_listing += names[0];

                for (const auto& name : names.subspan(1)) {
                    command_listing += ", " + name;
                }
            }

            if (command_listing.size() < column_size) {
                command_listing += std::string(column_size - command_listing.size(), ' ');
            }

            command_listing += cmd->description();

            display->write_stdout("    " + command_listing);
        }
    }
}

std::vector<std::string> form_names(std::string name, std::vector<std::string> aliases)
{
    aliases.insert(aliases.begin(), name);
    return aliases;
}

Console::Command::Command(
    std::string name, std::vector<std::string> aliases, std::function<void(argparse::ArgumentParser&)> builder)
    : m_names(form_names(name, aliases))
    , m_parser(name, "1.0", argparse::default_arguments::none)
{
    builder(m_parser);
}

std::span<const std::string> Console::Command::names() const { return m_names; }

std::string_view Console::Command::description() const { return m_parser.description(); }

void Console::Command::execute(const Console& console, std::vector<std::string> args) const
{
    argparse::ArgumentParser parser(m_parser);
    try {
        parser.parse_args(args);
    } catch (const std::runtime_error& err) {
        if (auto display = console.display().lock()) {
            display->write_stderr(err.what());
            display->write_stdout(parser.help().str());
        }

        return;
    }

    if (parser.is_configured("--help") && parser.get<bool>("--help")) {
        if (auto display = console.display().lock()) {
            display->write_stdout(parser.usage());
        }

        return;
    }

    this->execute(console, std::move(parser));
}
