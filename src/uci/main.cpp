#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <argparse/argparse.h>
#include <weechess/game_state.h>

#include "main.h"

struct UCI;

class UCIMoveQuery : public weechess::MoveQuery {
private:
    weechess::Location m_from;
    weechess::Location m_to;
    weechess::Piece::Type m_promotion;

public:
    UCIMoveQuery(
        weechess::Location from, weechess::Location to, weechess::Piece::Type promotion = weechess::Piece::Type::None)
        : m_from(from)
        , m_to(to)
        , m_promotion(promotion)
    {
    }

    bool test(const weechess::Move& move) const override
    {
        if (move.is_promotion() && move.promoted_piece_type() != m_promotion) {
            return false;
        }

        if (move.start_location() != m_from) {
            return false;
        }

        if (move.end_location() != m_to) {
            return false;
        }

        return true;
    }

    static std::optional<UCIMoveQuery> parse_from(std::string_view token)
    {
        if (token.size() < 4) {
            return {};
        }

        auto from = weechess::Location::from_string(token.substr(0, 2));
        if (!from)
            return {};

        auto to = weechess::Location::from_string(token.substr(2, 2));
        if (!to)
            return {};

        auto promotion = weechess::Piece::Type::None;
        if (token.size() == 5) {
            switch (token[4]) {
            case 'q':
                promotion = weechess::Piece::Type::Queen;
                break;
            case 'r':
                promotion = weechess::Piece::Type::Rook;
                break;
            case 'b':
                promotion = weechess::Piece::Type::Bishop;
                break;
            case 'n':
                promotion = weechess::Piece::Type::Knight;
                break;
            default:
                return {};
            }
        }

        return UCIMoveQuery(*from, *to, promotion);
    }
};

namespace utils {
std::string pop_token(std::istream& is)
{
    std::string token;
    is >> token;
    return token;
}

std::string consume(std::istream& is) { return std::string(std::istreambuf_iterator<char>(is), {}); }
}

struct UCI {
    bool in_debug_mode { false };
    weechess::GameState game_state { weechess::GameState::new_game() };

    void loop(std::istream& in, std::ostream& out, std::ostream& err = std::cerr);
};

struct UCICommand {
    std::string_view command;
    std::function<void(UCI&, std::istream&, std::ostream&, std::ostream& err)> handler;
};

const std::vector<UCICommand> commands = {
    UCICommand { "uci",
        [](UCI&, std::istream&, std::ostream& out, std::ostream&) {
            out << "id name weechess" << std::endl;
            out << "id author " WEECHESS_PROJECT_AUTHOR << std::endl;
            out << std::endl;
            out << "uciok" << std::endl;
        } },
    UCICommand { "debug",
        [](UCI& uci, std::istream& in, std::ostream& out, std::ostream&) {
            auto token = utils::pop_token(in);
            uci.in_debug_mode = token == "on";
        } },
    UCICommand { "isready",
        [](UCI&, std::istream&, std::ostream& out, std::ostream&) {
            out << "readyok" << std::endl;
            ;
        } },
    UCICommand { "position",
        [](UCI& uci, std::istream& in, std::ostream& out, std::ostream& err) {
            auto first_token = utils::pop_token(in);
            if (first_token != "startpos") {
                // Fen string
                std::string fen = first_token;
                for (;;) {
                    auto token = utils::pop_token(in);
                    if (token == "moves" || token.empty()) {
                        break;
                    }

                    fen += " " + token;
                }

                if (auto new_gs = weechess::GameState::from_fen(fen)) {
                    uci.game_state = *new_gs;
                } else {
                    err << "Invalid fen string: " << fen << std::endl;
                }
            } else {
                // Startpos
                uci.game_state = weechess::GameState::new_game();
                utils::pop_token(in); // Consume "moves"
            }

            // Moves
            for (;;) {
                auto token = utils::pop_token(in);
                if (token.empty()) {
                    break;
                }

                if (auto move_query = UCIMoveQuery::parse_from(token)) {
                    if (auto legal_move = uci.game_state.move_set().find_first(*move_query)) {
                        uci.game_state = weechess::GameState(legal_move->snapshot());
                    } else {
                        err << "Illegal move: " << token << std::endl;
                    }
                } else {
                    err << "Invalid move format: " << token << std::endl;
                }
            }

            err << "Position set to: " << uci.game_state.to_fen() << std::endl;
        } },
};

const std::vector<std::string> ignored_commands = {
    "ucinewgame",
    "setoption",
    "register",
};

void UCI::loop(std::istream& in, std::ostream& out, std::ostream& err)
{
    std::string line;
    std::string token;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        iss >> token;

        if (token == "quit") {
            break;
        }

        auto cmd = std::find_if(
            commands.begin(), commands.end(), [&](const UCICommand& cmd) { return cmd.command == token; });

        if (cmd != commands.end()) {
            cmd->handler(*this, iss, out, err);
            continue;
        }

        if (std::find(ignored_commands.begin(), ignored_commands.end(), token) != ignored_commands.end()) {
            continue;
        }

        err << "Unsupported command: " << token << std::endl;
    }
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser parser("weechess " UCI_CMD_NAME, UCI_CMD_VERSION, argparse::default_arguments::none);
    parser.add_description(UCI_CMD_DESCRIPTION);
    parser.add_argument("--help").default_value(false).implicit_value(true);

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

    UCI().loop(std::cin, std::cout);
}
