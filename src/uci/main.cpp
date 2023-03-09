#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <argparse/argparse.h>
#include <weechess/engine.h>
#include <weechess/game_state.h>
#include <weechess/threading.h>

#include "log.h"
#include "main.h"

template <typename T> std::istream& operator>>(std::istream& is, std::optional<T>& opt)
{
    T t;
    is >> t;
    opt = std::move(t);
    return is;
}

namespace utils {
std::string pop_token(std::istream& is)
{
    std::string token;
    is >> token;
    return token;
}

std::string consume(std::istream& is) { return std::string(std::istreambuf_iterator<char>(is), {}); }
}

struct UCIMove {
    weechess::Location from;
    weechess::Location to;
    weechess::Piece::Type promotion;

    static UCIMove from_move(const weechess::Move& move)
    {
        UCIMove umove {
            .from = move.start_location(),
            .to = move.end_location(),
            .promotion = weechess::Piece::Type::None,
        };

        if (move.is_promotion()) {
            umove.promotion = move.promoted_piece_type();
        }

        return umove;
    }
};

std::ostream& operator<<(std::ostream& os, const UCIMove& move)
{
    os << move.from << move.to;
    switch (move.promotion) {
    case weechess::Piece::Type::Queen:
        os << 'q';
        break;
    case weechess::Piece::Type::Rook:
        os << 'r';
        break;
    case weechess::Piece::Type::Bishop:
        os << 'b';
        break;
    case weechess::Piece::Type::Knight:
        os << 'n';
        break;
    default:
        break;
    }

    return os;
}

class UCISearchDelegate : public weechess::SearchDelegate {
private:
    std::ostream& m_out;

public:
    UCISearchDelegate(std::ostream& out)
        : m_out(out)
    {
    }

    void on_evaluation_event(const weechess::EvaluationEvent& event) override
    {
        m_out << "info score cp " << event.evaluation.score;
        m_out << " pv";
        for (const auto& move : event.best_line) {
            m_out << ' ' << UCIMove::from_move(move);
        }

        m_out << std::endl;
    }

    void on_performance_event(const weechess::PerformanceEvent& event) override
    {
        m_out << "info nodes " << event.nodes_searched;
        m_out << " depth " << event.current_depth;
        m_out << " nps " << event.nodes_per_second;
        m_out << " time " << event.elapsed_time.count();
        m_out << std::endl;
    }
};

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

struct UCI {
    bool in_debug_mode { false };
    weechess::GameState game_state { weechess::GameState::new_game() };
    weechess::threading::ThreadDispatcher dispatcher {};

    void loop(std::istream& in, std::ostream& out);
};

struct UCICommand {
    std::string_view command;
    std::function<void(UCI&, std::istream&, std::ostream&)> handler;
};

const std::vector<UCICommand> commands = {
    UCICommand { "uci",
        [](UCI&, std::istream&, std::ostream& out) {
            out << "id name weechess " << WEECHESS_PROJECT_VERSION << std::endl;
            out << "id author " WEECHESS_PROJECT_AUTHOR << std::endl;
            out << "uciok" << std::endl;
        } },
    UCICommand { "debug",
        [](UCI& uci, std::istream& in, std::ostream& out) {
            auto token = utils::pop_token(in);
            uci.in_debug_mode = token == "on";
        } },
    UCICommand { "isready",
        [](UCI&, std::istream&, std::ostream& out) {
            out << "readyok" << std::endl;
            ;
        } },
    UCICommand { "position",
        [](UCI& uci, std::istream& in, std::ostream& out) {
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
                    log::error("Invalid fen string: {}", fen);
                    return;
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
                        log::error("Illegal move: {}", token);
                    }
                } else {
                    log::error("Invalid move format: {}", token);
                }
            }

            log::debug("Position set to: {}", uci.game_state.to_fen());
        } },
    UCICommand { "go",
        [](UCI& uci, std::istream& in, std::ostream& out) {
            weechess::SearchParameters parameters;

            {
                std::string token;
                while (in >> token) {
                    if (token == "depth") {
                        in >> parameters.max_depth;
                    } else if (token == "nodes") {
                        in >> parameters.max_nodes;
                    } else if (token == "infinite") {
                        parameters.max_search_time = {};
                    } else if (token == "movetime") {
                        size_t time;
                        in >> time;
                        if (time > 0) {
                            parameters.max_search_time = std::chrono::milliseconds(time);
                        }
                    }
                }
            }

            uci.dispatcher.dispatch([&out, parameters, gs = uci.game_state](auto token) {
                UCISearchDelegate delegate(out);
                weechess::Engine engine;
                auto result = engine.calculate(gs, parameters, *token, delegate);

                if (result.best_line.size() > 0) {
                    out << "bestmove " << UCIMove::from_move(result.best_line[0]) << std::endl;
                } else {
                    out << "bestmove 0000" << std::endl;
                }
            });
        } },
    UCICommand { "stop",
        [](UCI& uci, std::istream& in, std::ostream& out) {
            uci.dispatcher.invalidate_all();
            uci.dispatcher.join_all();
        } },
};

const std::vector<std::string> ignored_commands = {
    "ucinewgame",
    "setoption",
    "register",
};

void UCI::loop(std::istream& in, std::ostream& out)
{
    std::string line;
    std::string token;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        iss >> token;

        if (token == "quit") {
            dispatcher.invalidate_all();
            dispatcher.join_all();
            break;
        }

        auto cmd = std::find_if(
            commands.begin(), commands.end(), [&](const UCICommand& cmd) { return cmd.command == token; });

        if (cmd != commands.end()) {
            cmd->handler(*this, iss, out);
            continue;
        }

        if (std::find(ignored_commands.begin(), ignored_commands.end(), token) != ignored_commands.end()) {
            continue;
        }

        log::error("Unsupported command: {}", token);
    }
}

int main(int argc, char* argv[])
{
    log::init_logging();
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

    std::cerr << "weechess-" UCI_CMD_NAME " v" UCI_CMD_VERSION << " by Ryan Webber..." << std::endl;

    UCI().loop(std::cin, std::cout);
}
