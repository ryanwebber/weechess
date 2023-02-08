#pragma once

#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace argparse {

    enum Cardinality {
        Single, Multiple
    };

    template<typename T>
    class Parser;

    template <typename T>
    struct Command {
        Parser<T> subcommand_parser;
    };

    template <typename T>
    struct Option {
        std::vector<std::string> names;
        std::string description;
        std::function<bool(T&, std::span<std::string>&)> callback;
    };

    template <typename T>
    struct Argument {
        Cardinality cardinality;
        std::vector<std::string> placeholders;
        std::function<bool(T&, std::span<std::string>&)> callback;
    };

    template <typename T>
    class Parser {
    public:
        Parser(std::string description);
        Parser(std::string name, std::string description);

        void add_command(Command<T>);
        void add_command(std::string name, std::string description, std::function<void(Parser<T>&)> builder);
        void add_option(Option<T>);
        void set_argument_placeholder(Argument<T>);

        std::string usage() const;
        std::vector<std::string> example_usages() const;

        bool parse(T&, int argc, char** argv);
        bool parse(T&, std::span<std::string>&);
    private:
        std::string m_name;
        std::string m_description;

        std::vector<Command<T>> m_commands;
        std::vector<Option<T>> m_options;
        std::optional<Argument<T>> m_pos_argument;
    };

    template <typename T>
    Parser<T>::Parser(std::string description)
        : Parser("", description) {}

    template <typename T>
    Parser<T>::Parser(std::string name, std::string description)
        : m_name(name)
        , m_description(description) {}

    template <typename T>
    void Parser<T>::add_command(Command<T> command) {
        m_commands.push_back(command);
    }

    template <typename T>
    void Parser<T>::add_command(std::string name, std::string description, std::function<void(Parser<T>&)> builder) {
        Parser<T> subcommand_parser(name, description);
        builder(subcommand_parser);
        m_commands.push_back({ subcommand_parser });
    }

    template <typename T>
    void Parser<T>::add_option(Option<T> option) {
        m_options.push_back(option);
    }

    template <typename T>
    void Parser<T>::set_argument_placeholder(Argument<T> argument) {
        m_pos_argument = argument;
    }

    template <typename T>
    std::vector<std::string> Parser<T>::example_usages() const {
        std::vector<std::string> usages;

        if (!m_name.empty()) {
            std::string usage = m_name;
            for (const auto& option : m_options) {
                if (option.names.empty()) {
                    continue;
                }

                usage += " [" + option.names[0] + "]";
            }

            if (m_pos_argument) {
                usage += " ";
                for (const auto& placeholder : m_pos_argument->placeholders) {
                    usage += " <" + placeholder + ">";
                }

                if (m_pos_argument->cardinality == Cardinality::Multiple) {
                    usage += " ...";
                }
            }

            usages.push_back(usage);
        }

        for (const auto& command : m_commands) {
            auto subcommand_usages = command.subcommand_parser.example_usages();
            for (auto& usage : subcommand_usages) {
                if (m_name.empty())
                    usages.push_back(usage);
                else
                    usages.push_back(m_name + " " + usage);
            }
        }

        return usages;
    }

    template <typename T>
    std::string Parser<T>::usage() const {
        return "TODO";
    }

    template <typename T>
    bool Parser<T>::parse(T& result, int argc, char** argv) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        return parse(result, args);
    }

    template <typename T>
    bool Parser<T>::parse(T& result, std::span<std::string>& args) {
        return false;
    }
}
