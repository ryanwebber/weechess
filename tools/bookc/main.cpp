#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>

#include <argparse/argparse.h>
#include <weechess/game_state.h>
#include <weechess/move.h>
#include <weechess/move_query.h>
#include <weechess/zobrist.h>

using namespace weechess;

constexpr size_t max_opening_moves = 16;

struct MoveEntry {
    std::string string;
    PGNMoveQuery query;
};

struct MoveCompare {
    bool operator()(const Move& a, const Move& b) const { return a.data().to_ulong() < b.data().to_ulong(); }
};

void add_moves(const std::vector<MoveEntry>& entries, std::map<zobrist::Hash, std::set<Move, MoveCompare>>& dict)
{
    auto game_state = GameState::new_game();
    for (auto i = 0; i < std::min(entries.size(), max_opening_moves); ++i) {
        const auto& entry = entries[i];
        auto possible_moves = game_state.move_set().find(entry.query);
        if (possible_moves.empty()) {
            std::cout << "Failed to find move: " << entry.string << " at position: " << game_state.to_fen()
                      << std::endl;
            exit(1);
        } else if (possible_moves.size() > 1) {
            std::cout << "Ambiguous move: " << entry.string << " at position: " << game_state.to_fen() << std::endl;
            exit(1);
        }

        auto move = possible_moves[0].move();
        auto hash = game_state.snapshot().zobrist_hash();
        dict[hash].insert(move);

        game_state = GameState(possible_moves[0].snapshot());
    }
}

std::vector<MoveEntry> parse_moves(std::string input)
{
    std::istringstream is(input);

    std::string token;
    std::vector<std::string> move_strings;

    while (is.good()) {
        is >> token;

        if (token.empty()) {
            continue;
        } else if (token.back() == '.') {
            continue;
        } else if (token.find('.') != std::string::npos) {
            token.erase(0, token.find('.') + 1);
        }

        move_strings.push_back(token);

        if (token == "1/2-1/2" || token == "1-0" || token == "0-1") {
            break;
        }
    }

    if (move_strings.size() > 0) {
        // Skip over the last move, which is actually the result of the game
        move_strings.pop_back();
    }

    std::vector<MoveEntry> moves;

    for (const auto& move_string : move_strings) {
        auto move_query = PGNMoveQuery::from(move_string);
        if (!move_query.has_value()) {
            std::cout << "Failed to parse move: " << move_string << std::endl;
            exit(1);
        }

        moves.push_back({ move_string, *move_query });
    }

    return moves;
}

int main(int argc, const char* argv[])
{
    argparse::ArgumentParser parser("bookc", WEECHESS_PROJECT_VERSION, argparse::default_arguments::none);
    parser.add_description("Compile book files for weechess from PGN archives");
    parser.add_argument("--help").default_value(false).implicit_value(true);
    parser.add_argument("archives").remaining();

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    std::vector<std::string> input_files;
    try {
        input_files = parser.get<std::vector<std::string>>("archives");
    } catch (std::logic_error& e) {
        std::cout << "No book files provided." << std::endl;
        std::cout << parser;
        std::exit(1);
    }

    if (parser.get<bool>("--help")) {
        std::cout << parser;
        std::exit(0);
    }

    std::map<zobrist::Hash, std::set<Move, MoveCompare>> book;

    for (const auto& filename : input_files) {
        std::cerr << "Processing file: " << filename << std::endl;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << std::endl;
            std::exit(1);
        }

        std::string line;
        std::string move_list_section;
        while (std::getline(file, line)) {
            if (line.empty())
                continue;

            if (line[0] == '[') {
                if (move_list_section.empty()) {
                    continue;
                } else {
                    auto move_entries = parse_moves(move_list_section);
                    add_moves(move_entries, book);
                    move_list_section.clear();
                }
            } else {
                move_list_section += " " + line;
            }
        }
    }

    std::vector<Move> moves;

    std::cout << "#include \"book_data.h\"" << std::endl;
    std::cout << std::endl;

    std::cout << "namespace weechess::generated {" << std::endl;
    std::cout << "constexpr std::array<Book::Entry, " << book.size() << "> entries = { {" << std::endl;

    size_t offset = 0;
    for (const auto& [key, value] : book) {
        std::cout << "    { " << key << "ULL, " << offset << ", " << value.size() << " }," << std::endl;
        offset += value.size();
        moves.insert(moves.end(), value.begin(), value.end());
    }
    std::cout << "} };" << std::endl;
    std::cout << std::endl;

    std::cout << "constexpr std::array<Move, " << moves.size() << "> moves = {" << std::endl;
    for (const auto& move : moves) {
        std::cout << "    Move(Move::Data(" << move.data().to_ulong() << "UL))," << std::endl;
    }
    std::cout << "};" << std::endl;

    std::cout << std::endl;
    std::cout << "const Book::Data book_data = { entries, moves };" << std::endl;
    std::cout << "}" << std::endl;
}
