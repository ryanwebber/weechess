#include <iostream>
#include <regex>
#include "game_state.h"

namespace weechess {

    namespace fen {
        constexpr const char* regex_string =
            R"(^(((?:[rnbqkpRNBQKP1-8]+\/){7})[rnbqkpRNBQKP1-8]+)\s([b|w])\s([K|Q|k|q]{1,4})\s(-|[a-h][1-8])\s(\d+\s\d+)$)";

        namespace groups {
            constexpr size_t count = 7;

            constexpr size_t board = 1;
            constexpr size_t turn_to_move = 3;
            constexpr size_t castle_rights = 4;
            constexpr size_t en_passant_target = 5;
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
    }

    constexpr std::string_view default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    std::optional<Board> board_from_fen_fragment(std::string_view fragment);
    PlayerState<CastleRights> castle_rights_from_fen_fragment(std::string_view fragment);
    std::optional<Location> location_from_fen_fragment(std::string_view fragment);
    std::optional<Piece> piece_from_fen(char c);
    std::optional<char> piece_to_fen(const Piece &piece);

    bool CastleRights::has_rights() const {
        return can_castle_kingside || can_castle_queensize;
    }

    CastleRights CastleRights::none() {
        return CastleRights { false, false };
    }

    CastleRights CastleRights::all() {
        return CastleRights { true, true };
    }

    GameState::GameState()
        : m_turn_to_move(Color::White)
        , m_castle_rights(CastleRights::all())
        , m_en_passant_target(std::nullopt) {}

    GameState::GameState(
        Board board,
        Color turn_to_move,
        PlayerState<CastleRights> castle_rights,
        std::optional<Location> en_passant_target
    )
        : m_board(board)
        , m_turn_to_move(turn_to_move)
        , m_castle_rights(castle_rights)
        , m_en_passant_target(en_passant_target) {}

    GameState GameState::new_game() {
        return GameState::from_fen(default_fen).value();
    }

    std::string GameState::to_fen() const {

        auto cells = m_board.cells();

        std::string fen;
        uint8_t space_count = 0;
        for (size_t i = 0; i < Board::cell_count; i++) {

            if (i % 8 == 0 && i != 0) {
                if (space_count > 0) {
                    fen += std::to_string(space_count);
                    space_count = 0;
                }

                fen += '/';
            }

            auto fen_char = piece_to_fen(cells[i]);
            if (fen_char.has_value()) {
                if (space_count > 0) {
                    fen += std::to_string(space_count);
                    space_count = 0;
                }

                fen += fen_char.value();
            } else {
                space_count++;
            }
        }

        fen += " ";
        fen += m_turn_to_move == Color::White ? 'w' : 'b';

        fen += " ";
        if (m_castle_rights[Color::White].has_rights() || m_castle_rights[Color::Black].has_rights()) {
            if (m_castle_rights[Color::White].can_castle_kingside)
                fen += 'K';
            if (m_castle_rights[Color::White].can_castle_queensize)
                fen += 'Q';
            if (m_castle_rights[Color::Black].can_castle_kingside)
                fen += 'k';
            if (m_castle_rights[Color::Black].can_castle_queensize)
                fen += 'q';
        } else {
            fen += "-";
        }

        fen += " ";
        if (m_en_passant_target.has_value()) {
            fen += m_en_passant_target.value().to_string();
        } else {
            fen += "-";
        }

        fen += " 0 1";

        return fen;
    }

    std::optional<GameState> GameState::from_fen(std::string_view fen_sv) {

        std::regex re(fen::regex_string);
        std::match_results<std::string_view::const_iterator> match;
        auto flags = std::regex_constants::match_default;
        
        if (!std::regex_match(fen_sv.begin(), fen_sv.end(), match, re, flags)) {
            return std::nullopt;
        }

        if (match.size() != fen::groups::count) {
            return std::nullopt;
        }

        std::string board_string(match[fen::groups::board].str());
        std::string turn_to_move_string(match[fen::groups::turn_to_move].str());
        std::string castle_rights_string(match[fen::groups::castle_rights].str());
        std::string en_passant_target_string(match[fen::groups::en_passant_target].str());

        Board board = board_from_fen_fragment(board_string).value();
        Color turn_to_move = turn_to_move_string == "w" ? Color::White : Color::Black;
        PlayerState<CastleRights> castle_rights = castle_rights_from_fen_fragment(castle_rights_string);
        std::optional<Location> en_passant_target = location_from_fen_fragment(en_passant_target_string);

        return GameState(board, turn_to_move, castle_rights, en_passant_target); 
    }

    std::optional<Board> board_from_fen_fragment(std::string_view fragment) {
        Board::Buffer cells;
        size_t i = 0;
        for (char c : fragment) {
            std::optional<Piece> piece = piece_from_fen(c);
            if (c == ' ') {
                break;
            } else if (c == '/') {
                continue;
            } else if (std::isdigit(c)) {
                i += (c - '0');
            } else if (piece.has_value()) {
                cells[i] = piece.value();
                i++;
            } else {
                return std::nullopt;
            }
        }

        return Board(cells);
    }

    PlayerState<CastleRights> castle_rights_from_fen_fragment(std::string_view fragment) {
        PlayerState<CastleRights> castle_rights(CastleRights::none());
        for (char c : fragment) {
            switch (c) {
                case fen::white_king: castle_rights[Color::White].can_castle_kingside = true; break;
                case fen::black_king: castle_rights[Color::Black].can_castle_kingside = true; break;
                case fen::white_queen: castle_rights[Color::White].can_castle_queensize = true; break;
                case fen::black_queen: castle_rights[Color::Black].can_castle_queensize = true; break;
            }
        }

        return castle_rights;
    }


    std::optional<Location> location_from_fen_fragment(std::string_view fragment) {
        if (fragment.size() != 2) {
            return std::nullopt;
        }

        char file = fragment[0];
        char rank = fragment[1];

        if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
            return std::nullopt;
        }

        return Location(file - 'a', rank - '1');
    }

    std::optional<Piece> piece_from_fen(char c) {
        switch (c) {
            case fen::white_pawn: return Piece(PieceType::Pawn, Color::White);
            case fen::white_knight: return Piece(PieceType::Knight, Color::White);
            case fen::white_bishop: return Piece(PieceType::Bishop, Color::White);
            case fen::white_rook: return Piece(PieceType::Rook, Color::White);
            case fen::white_queen: return Piece(PieceType::Queen, Color::White);
            case fen::white_king: return Piece(PieceType::King, Color::White);
            case fen::black_pawn: return Piece(PieceType::Pawn, Color::Black);
            case fen::black_knight: return Piece(PieceType::Knight, Color::Black);
            case fen::black_bishop: return Piece(PieceType::Bishop, Color::Black);
            case fen::black_rook: return Piece(PieceType::Rook, Color::Black);
            case fen::black_queen: return Piece(PieceType::Queen, Color::Black);
            case fen::black_king: return Piece(PieceType::King, Color::Black);
            default: return std::nullopt;
        }
    }

    std::optional<char> piece_to_fen(const Piece &piece) {
        if (piece.is(PieceType::Pawn)) {
            return piece.is(Color::White) ? fen::white_pawn : fen::black_pawn;
        } else if (piece.is(PieceType::Knight)) {
            return piece.is(Color::White) ? fen::white_knight : fen::black_knight;
        } else if (piece.is(PieceType::Bishop)) {
            return piece.is(Color::White) ? fen::white_bishop : fen::black_bishop;
        } else if (piece.is(PieceType::Rook)) {
            return piece.is(Color::White) ? fen::white_rook : fen::black_rook;
        } else if (piece.is(PieceType::Queen)) {
            return piece.is(Color::White) ? fen::white_queen : fen::black_queen;
        } else if (piece.is(PieceType::King)) {
            return piece.is(Color::White) ? fen::white_king : fen::black_king;
        } else {
            return std::nullopt;
        }
    }
}
