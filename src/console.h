#include <memory>
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
            virtual void write_stdout(std::string) = 0;
            virtual void write_stderr(std::string) = 0;

            virtual ~Display() = default;
        };

        class Service {
        public:
            virtual bool cmd_move_piece(weechess::Move) = 0;
            virtual ~Service() = default;
        };

        Console() = default;
        Console(std::weak_ptr<Display>, std::weak_ptr<Service>);

        void set_display(std::weak_ptr<Display>);
        void set_service(std::weak_ptr<Service>);

        void execute(std::string_view command);
    
    private:
        struct CommandParser {
            std::vector<std::string> names;
            argparse::ArgumentParser parser;

            CommandParser(std::vector<std::string> names, argparse::ArgumentParser parser)
                : names(names)
                , parser(parser) {}
        };

        std::vector<CommandParser> m_commands;
        std::weak_ptr<Display> m_display;
        std::weak_ptr<Service> m_service;

        void initialize();
        void print_usage() const;
};
