#include "game_state.h"

namespace weechess {
    
    constexpr std::string_view default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    CastleRights CastleRights::none() {
        return CastleRights { false, false };
    }

    CastleRights CastleRights::all() {
        return CastleRights { true, true };
    }

    GameState::GameState()
        : m_turn_to_move(Color::White)
        , m_castle_rights(CastleRights::all()) {}

    GameState::GameState(Board board, Color turn_to_move, PlayerState<CastleRights> castle_rights)
        : m_board(board)
        , m_turn_to_move(turn_to_move)
        , m_castle_rights(castle_rights) {}

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

            auto fen_char = Piece(cells[i]).to_fen();
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

        return fen;
    }

    std::optional<GameState> GameState::from_fen(std::string_view fen) {
        std::array<Piece, Board::cell_count> cells;
        size_t i = 0;
        for (char c : fen) {
            std::optional<Piece> piece = Piece::from_fen(c);
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

        Board board(cells);
        Color turn_to_move = Color::White;
        PlayerState<CastleRights> castle_rights(CastleRights::all());

        return GameState(board, turn_to_move, castle_rights); 
    }

    GameState GameState::new_game() {
        return GameState::from_fen(default_fen).value();
    }
}
