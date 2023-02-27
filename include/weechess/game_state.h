#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <weechess/board.h>
#include <weechess/color_map.h>
#include <weechess/move.h>
#include <weechess/move_query.h>
#include <weechess/piece.h>

namespace weechess {

class GameState;

struct CastleRights {
    bool can_castle_kingside { true };
    bool can_castle_queenside { true };

    bool has_rights() const;

    static constexpr CastleRights none() { return { false, false }; };
    static constexpr CastleRights all() { return { true, true }; };
};

class MoveSet {
public:
    MoveSet() = default;
    MoveSet(std::vector<Move> legal_moves);

    std::span<const Move> legal_moves() const;
    std::vector<Move> legal_moves_from(Location) const;
    bool is_legal_move(const Move&) const;

    std::vector<Move> find(const MoveQuery&) const;

    static MoveSet compute_from(const GameState&);

private:
    std::vector<Move> m_legal_moves;
};

class GameState {
public:
    GameState();
    GameState(Board board,
        Color turn_to_move,
        ColorMap<CastleRights> castle_rights,
        std::optional<Location> en_passant_target,
        size_t halfmove_clock,
        size_t fullmove_number);

    const Board& board() const;
    const Color& turn_to_move() const;
    const ColorMap<CastleRights>& castle_rights() const;
    const std::optional<Location>& en_passant_target() const;

    size_t halfmove_clock() const;
    size_t fullmove_number() const;

    bool is_check() const;
    bool is_checkmate() const;
    bool is_stalemate() const;

    const MoveSet& move_set() const;

    std::string san_notation(const Move&) const;
    std::string verbose_description(const Move&) const;

    std::string to_fen() const;

    static std::optional<GameState> from_fen(std::string_view);
    static GameState new_game();
    static std::optional<GameState> by_performing_move(const GameState&, const Move&);
    static std::optional<GameState> by_performing_moves(const GameState&, std::span<const Move>);

private:
    Board m_board;
    Color m_turn_to_move;

    std::optional<MoveSet> m_move_set {};

    ColorMap<CastleRights> m_castle_rights;
    std::optional<Location> m_en_passant_target;

    size_t m_halfmove_clock;
    size_t m_fullmove_number;
};

}
