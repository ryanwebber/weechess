#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <weechess/board.h>
#include <weechess/move.h>
#include <weechess/piece.h>
#include <weechess/player_state.h>

namespace weechess {

struct MoveDetail {
    std::optional<Piece> capture;
};

struct CastleRights {
    bool can_castle_kingside;
    bool can_castle_queensize;

    bool has_rights() const;

    static CastleRights none();
    static CastleRights all();
};

class GameState {
public:
    class Analysis {
    private:
        bool m_is_check;
        std::vector<Move> m_legal_moves;
        std::array<uint8_t, Board::cell_count> m_threat_map;
        std::array<std::span<const Move>, Board::cell_count> m_legal_moves_by_location;

    public:
        /*
         * Note: the legal moves vector must contain contiguous moves from the same origin,
         * so that as an optimization we can quickly iterate over moves from each location.
         */
        Analysis(bool is_check, std::vector<Move> legal_moves, std::array<uint8_t, Board::cell_count> threat_map);

        bool is_check() const;
        bool is_checkmate() const;
        bool is_stalemate() const;

        bool is_legal_move(const Move) const;

        std::span<const Move> legal_moves() const;
        std::span<const Move> legal_moves_from(const Location) const;

        std::span<const uint8_t> threat_map() const;
    };

    GameState();
    GameState(Board board,
        Color turn_to_move,
        PlayerState<CastleRights> castle_rights,
        std::optional<Location> en_passant_target);

    const Board& board() const;
    const Color& turn_to_move() const;
    const PlayerState<CastleRights>& castle_rights() const;
    const std::optional<Location>& en_passant_target() const;

    const Analysis& analysis() const;

    GameState as_monochromatic(Color perspective) const;

    std::string to_fen() const;
    static std::optional<GameState> from_fen(std::string_view);
    static GameState new_game();

    static std::optional<GameState> by_performing_move(const GameState&, const Move&, MoveDetail*);

private:
    Board m_board;
    Color m_turn_to_move;

    PlayerState<CastleRights> m_castle_rights;
    std::optional<Location> m_en_passant_target;

    std::optional<Analysis> m_analysis;
};

}
