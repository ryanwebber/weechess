#include <cstdlib>
#include <string>
#include <vector>

#include <argparse/argparse.h>

#include "play/main.h"
#include "uci/main.h"

int run_subcommand(int argc, char* argv[], argparse::ArgumentParser parser, argparse::ArgumentParser sub_parser)
{
    std::string cmd = std::string(argv[0]) + "-" + std::string(sub_parser.name());

    if (sub_parser.get<bool>("--help") || parser.get<bool>("--help")) {
        cmd += " --help";
    } else {
        for (int i = 2; i < argc; ++i) {
            cmd += " ";
            cmd += argv[i];
        }
    }

    return std::system(cmd.c_str());
}

int main(int argc, char* argv[])
{

    argparse::ArgumentParser parser(argv[0], WEECHESS_PROJECT_VERSION, argparse::default_arguments::none);
    parser.add_description("A chess engine for the terminal");
    parser.add_argument("--help").default_value(false).implicit_value(true);

    // Sub parsers

    argparse::ArgumentParser play_parser(PLAY_CMD_NAME, PLAY_CMD_DESCRIPTION, argparse::default_arguments::none);
    play_parser.add_description(PLAY_CMD_DESCRIPTION);
    play_parser.add_argument("--help").default_value(false).implicit_value(true);
    parser.add_subparser(play_parser);

    argparse::ArgumentParser uci_parser(UCI_CMD_NAME, UCI_CMD_VERSION, argparse::default_arguments::none);
    uci_parser.add_description(UCI_CMD_DESCRIPTION);
    uci_parser.add_argument("--help").default_value(false).implicit_value(true);
    parser.add_subparser(uci_parser);

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    // There's a bug in the argparser that means we can't use arrays here, annoyingly
    if (parser.is_subcommand_used(PLAY_CMD_NAME)) {
        return run_subcommand(argc, argv, parser, play_parser);
    } else if (parser.is_subcommand_used(UCI_CMD_NAME)) {
        return run_subcommand(argc, argv, parser, uci_parser);
    }

    std::cerr << parser;
    return 0;
}
