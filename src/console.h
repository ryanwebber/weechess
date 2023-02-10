#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <weechess/move.h>

#include "argparse.h"

class Console {
public:
    class Display {
    public:
        virtual void exit() = 0;
        virtual void clear() = 0;
        virtual void write_stdout(std::string) = 0;
        virtual void write_stderr(std::string) = 0;

        virtual ~Display() = default;
    };

    class Service {
    public:
        virtual bool cmd_move_piece(weechess::Move) = 0;
        virtual ~Service() = default;
    };

    class Command {
    private:
        std::vector<std::string> m_names;
        argparse::ArgumentParser m_parser;

    protected:
        virtual void execute(const Console& console, argparse::ArgumentParser parser) const = 0;

    public:
        Command(std::string name, std::vector<std::string> aliases, std::function<void(argparse::ArgumentParser&)>);

        std::span<const std::string> names() const;
        std::string_view description() const;

        void execute(const Console& console, std::vector<std::string> args) const;

        virtual ~Command() = default;
    };

    Console();

    std::weak_ptr<Display> display() const;
    void set_display(std::weak_ptr<Display>);

    std::weak_ptr<Service> service() const;
    void set_service(std::weak_ptr<Service>);

    void print_usage() const;

    void execute(std::string_view command);

private:
    std::vector<std::unique_ptr<Command>> m_commands;
    std::weak_ptr<Display> m_display;
    std::weak_ptr<Service> m_service;

    friend class CommandParser;
};
