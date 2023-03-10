#include <iostream>
#include <regex>

#include <weechess/color_map.h>
#include <weechess/location.h>
#include <weechess/piece.h>

#include "fen.h"

namespace weechess::fen {
constexpr const char* regex_string
    = R"(^(((?:[rnbqkpRNBQKP1-8]+\/){7})[rnbqkpRNBQKP1-8]+)\s([b|w])\s(-|([K|Q|k|q]{1,4}))\s(-|[a-h][1-8])\s(\d+)\s(\d+)$)";

namespace groups {
    constexpr size_t count = 9;

    constexpr size_t board = 1;
    constexpr size_t turn_to_move = 3;
    constexpr size_t castle_rights = 4;
    constexpr size_t en_passant_target = 6;
    constexpr size_t half_move_clock = 7;
    constexpr size_t full_move_number = 8;
}

constexpr char white_pawn = 'P';
constexpr char white_knight = 'N';
constexpr char white_bishop = 'B';
constexpr char white_rook = 'R';
constexpr char white_queen = 'Q';
constexpr char white_king = 'K';
constexpr char black_pawn = 'p';
constexpr char black_knight = 'n';
constexpr char black_bishop = 'b';
constexpr char black_rook = 'r';
constexpr char black_queen = 'q';
constexpr char black_king = 'k';

std::optional<Board> board_from_fen_fragment(std::string_view fragment);
ColorMap<CastleRights> castle_rights_from_fen_fragment(std::string_view fragment);
std::optional<Location> location_from_fen_fragment(std::string_view fragment);
std::optional<Piece> piece_from_fen(char c);
std::optional<char> piece_to_fen(const Piece& piece);

std::string to_fen(const GameSnapshot& snapshot)
{
    const auto& cells = snapshot.board.to_array();
    const auto& turn_to_move = snapshot.turn_to_move;
    const auto& castle_rights = snapshot.castle_rights;
    const auto& en_passant_target = snapshot.en_passant_target;

    std::string fen_str;
    uint8_t space_count = 0;
    for (size_t i = 0; i < Board::cell_count; i++) {

        Location l(i);
        Location corrected = Location::from_rank_and_file(7 - l.rank(), l.file());

        if (i % 8 == 0 && i != 0) {
            if (space_count > 0) {
                fen_str += std::to_string(space_count);
                space_count = 0;
            }

            fen_str += '/';
        }

        auto fen_char = piece_to_fen(cells[corrected.offset]);
        if (fen_char.has_value()) {
            if (space_count > 0) {
                fen_str += std::to_string(space_count);
                space_count = 0;
            }

            fen_str += fen_char.value();
        } else {
            space_count++;
        }
    }

    if (space_count > 0) {
        fen_str += std::to_string(space_count);
    }

    fen_str += " ";
    fen_str += turn_to_move == Color::White ? 'w' : 'b';

    fen_str += " ";
    if (castle_rights[Color::White].has_rights() || castle_rights[Color::Black].has_rights()) {
        if (castle_rights[Color::White].can_castle_kingside)
            fen_str += 'K';
        if (castle_rights[Color::White].can_castle_queenside)
            fen_str += 'Q';
        if (castle_rights[Color::Black].can_castle_kingside)
            fen_str += 'k';
        if (castle_rights[Color::Black].can_castle_queenside)
            fen_str += 'q';
    } else {
        fen_str += "-";
    }

    fen_str += " ";
    if (en_passant_target.has_value()) {
        fen_str += en_passant_target.value().to_string();
    } else {
        fen_str += "-";
    }

    fen_str += " " + std::to_string(snapshot.halfmove_clock);
    fen_str += " " + std::to_string(snapshot.fullmove_number);

    return fen_str;
}

std::optional<GameSnapshot> from_fen(std::string_view fen_sv)
{
    std::regex re(regex_string);
    std::match_results<std::string_view::const_iterator> match;
    auto flags = std::regex_constants::match_default;

    if (!std::regex_match(fen_sv.begin(), fen_sv.end(), match, re, flags)) {
        return {};
    }

    if (match.size() != groups::count) {
        return {};
    }

    std::string board_string(match[groups::board].str());
    std::string turn_to_move_string(match[groups::turn_to_move].str());
    std::string castle_rights_string(match[groups::castle_rights].str());
    std::string en_passant_target_string(match[groups::en_passant_target].str());
    std::string half_move_clock_string(match[groups::half_move_clock].str());
    std::string full_move_number_string(match[groups::full_move_number].str());

    Board board = board_from_fen_fragment(board_string).value();
    Color turn_to_move = turn_to_move_string[0] == 'w' ? Color::White : Color::Black;
    ColorMap<CastleRights> castle_rights = castle_rights_from_fen_fragment(castle_rights_string);
    std::optional<Location> en_passant_target = location_from_fen_fragment(en_passant_target_string);

    size_t half_move_clock = std::stoul(half_move_clock_string);
    size_t full_move_number = std::stoul(full_move_number_string);

    return GameSnapshot(board, turn_to_move, castle_rights, en_passant_target, half_move_clock, full_move_number);
}

std::optional<Board> board_from_fen_fragment(std::string_view fragment)
{
    Board::Builder builder;
    size_t i = 0;
    for (char c : fragment) {
        if (c == ' ') {
            break;
        } else if (c == '/') {
            continue;
        } else if (std::isdigit(c)) {
            i += (c - '0');
        } else if (auto piece = piece_from_fen(c)) {
            Location l(i);
            Location corrected = Location::from_rank_and_file(7 - l.rank(), l.file());
            builder[corrected.offset] = piece.value();
            i++;
        } else {
            return {};
        }
    }

    return builder.build();
}

ColorMap<CastleRights> castle_rights_from_fen_fragment(std::string_view fragment)
{
    ColorMap<CastleRights> castle_rights(CastleRights::none());
    for (char c : fragment) {
        switch (c) {
        case white_king:
            castle_rights[Color::White].can_castle_kingside = true;
            break;
        case black_king:
            castle_rights[Color::Black].can_castle_kingside = true;
            break;
        case white_queen:
            castle_rights[Color::White].can_castle_queenside = true;
            break;
        case black_queen:
            castle_rights[Color::Black].can_castle_queenside = true;
            break;
        }
    }

    return castle_rights;
}

std::optional<Location> location_from_fen_fragment(std::string_view fragment)
{
    if (fragment.size() != 2) {
        return std::nullopt;
    }

    char file = fragment[0];
    char rank = fragment[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
        return {};
    }

    return Location::from_rank_and_file(rank - '1', file - 'a');
}

std::optional<Piece> piece_from_fen(char c)
{
    switch (c) {
    case white_pawn:
        return Piece(Piece::Type::Pawn, Color::White);
    case white_knight:
        return Piece(Piece::Type::Knight, Color::White);
    case white_bishop:
        return Piece(Piece::Type::Bishop, Color::White);
    case white_rook:
        return Piece(Piece::Type::Rook, Color::White);
    case white_queen:
        return Piece(Piece::Type::Queen, Color::White);
    case white_king:
        return Piece(Piece::Type::King, Color::White);
    case black_pawn:
        return Piece(Piece::Type::Pawn, Color::Black);
    case black_knight:
        return Piece(Piece::Type::Knight, Color::Black);
    case black_bishop:
        return Piece(Piece::Type::Bishop, Color::Black);
    case black_rook:
        return Piece(Piece::Type::Rook, Color::Black);
    case black_queen:
        return Piece(Piece::Type::Queen, Color::Black);
    case black_king:
        return Piece(Piece::Type::King, Color::Black);
    default:
        return {};
    }
}

std::optional<char> piece_to_fen(const Piece& piece)
{
    if (piece.is(Piece::Type::Pawn)) {
        return piece.is(Color::White) ? white_pawn : black_pawn;
    } else if (piece.is(Piece::Type::Knight)) {
        return piece.is(Color::White) ? white_knight : black_knight;
    } else if (piece.is(Piece::Type::Bishop)) {
        return piece.is(Color::White) ? white_bishop : black_bishop;
    } else if (piece.is(Piece::Type::Rook)) {
        return piece.is(Color::White) ? white_rook : black_rook;
    } else if (piece.is(Piece::Type::Queen)) {
        return piece.is(Color::White) ? white_queen : black_queen;
    } else if (piece.is(Piece::Type::King)) {
        return piece.is(Color::White) ? white_king : black_king;
    } else {
        return {};
    }
}
}
