#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <weechess/board.h>
#include <weechess/color_map.h>
#include <weechess/move.h>
#include <weechess/piece.h>

namespace weechess {

struct MoveDetail {
    std::optional<Piece> capture;
};

struct CastleRights {
    bool can_castle_kingside { true };
    bool can_castle_queenside { true };

    bool has_rights() const;

    static constexpr CastleRights none() { return { false, false }; };
    static constexpr CastleRights all() { return { true, true }; };
};

class GameState {
public:
    class Analysis {
    private:
        bool m_is_check;
        std::vector<Move> m_legal_moves;
        std::array<std::span<const Move>, Board::cell_count> m_legal_moves_by_location;

    public:
        /*
         * Note: the legal moves vector must contain contiguous moves from the same origin,
         * so that as an optimization we can quickly iterate over moves from each location.
         */
        Analysis(bool is_check, std::vector<Move> legal_moves);

        bool is_check() const;
        bool is_checkmate() const;
        bool is_stalemate() const;

        bool is_legal_move(const Move) const;

        std::span<const Move> legal_moves() const;
        std::span<const Move> legal_moves_from(const Location) const;

        static Analysis from(const GameState&);
    };

    GameState();
    GameState(Board board,
        Color turn_to_move,
        ColorMap<CastleRights> castle_rights,
        std::optional<Location> en_passant_target);

    const Board& board() const;
    const Color& turn_to_move() const;
    const ColorMap<CastleRights>& castle_rights() const;
    const std::optional<Location>& en_passant_target() const;

    std::span<const Location> piece_locations() const;

    const Analysis& analysis() const;

    GameState as_monochromatic(Color perspective) const;

    std::string to_fen() const;
    static std::optional<GameState> from_fen(std::string_view);
    static GameState new_game();

    static std::optional<GameState> by_performing_move(const GameState&, const Move&, MoveDetail*);

private:
    Board m_board;
    Color m_turn_to_move;

    ColorMap<CastleRights> m_castle_rights;
    std::optional<Location> m_en_passant_target;

    std::vector<Location> m_piece_locations;
    std::optional<Analysis> m_analysis;

    bool m_analysis_disabled { false };

    friend class Analysis;
};

}
