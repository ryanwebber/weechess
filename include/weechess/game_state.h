#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <weechess/board.h>
#include <weechess/color_map.h>
#include <weechess/move.h>
#include <weechess/move_query.h>
#include <weechess/piece.h>

namespace weechess {

struct CastleRights {
    bool can_castle_kingside { true };
    bool can_castle_queenside { true };

    bool has_rights() const;

    static constexpr CastleRights none() { return { false, false }; };
    static constexpr CastleRights all() { return { true, true }; };
};

struct GameSnapshot {
    Board board;
    Color turn_to_move;

    ColorMap<CastleRights> castle_rights;
    std::optional<Location> en_passant_target;

    size_t halfmove_clock;
    size_t fullmove_number;

    GameSnapshot() = default;
    GameSnapshot(Board board,
        Color turn_to_move,
        ColorMap<CastleRights> castle_rights,
        std::optional<Location> en_passant_target,
        size_t halfmove_clock,
        size_t fullmove_number);

    std::optional<GameSnapshot> by_performing_move(const Move&) const;
    std::optional<GameSnapshot> by_performing_moves(std::span<const std::shared_ptr<MoveQuery>>) const;

    std::string to_fen() const;
    static std::optional<GameSnapshot> from_fen(std::string_view);

    static GameSnapshot initial_position();

private:
    static std::optional<Board> augmented_board_for_move(const GameSnapshot&, const Move&);
    static std::optional<GameSnapshot> by_performing_move(const GameSnapshot&, const Move&);
    static std::optional<GameSnapshot> by_performing_moves(
        const GameSnapshot&, std::span<const std::shared_ptr<MoveQuery>>);
};

class LegalMove {
public:
    const Move& move() const;
    const GameSnapshot& snapshot() const;

    LegalMove(Move, GameSnapshot);

    const Move& operator*() const;
    const Move* operator->() const;

private:
    Move m_move;
    GameSnapshot m_snapshot;
};

class MoveSet {
public:
    MoveSet() = default;
    MoveSet(std::vector<LegalMove> legal_moves);

    std::span<const LegalMove> legal_moves() const;

    std::vector<LegalMove> legal_moves_from(Location) const;
    std::vector<LegalMove> find(const MoveQuery&) const;
    std::optional<LegalMove> find_first(const MoveQuery&) const;
    std::optional<LegalMove> find(const Move&) const;

    static MoveSet compute(const GameSnapshot&);

private:
    std::vector<LegalMove> m_legal_moves;
};

class GameState {
public:
    GameState();
    GameState(GameSnapshot snapshot);

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

    const GameSnapshot& snapshot() const;

    static std::optional<GameState> from_fen(std::string_view);
    static GameState new_game();

private:
    GameSnapshot m_snapshot;
    std::optional<MoveSet> m_move_set {};
};

}
