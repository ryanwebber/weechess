#pragma once

#include <array>
#include <bitset>
#include <optional>
#include <string>

#include <weechess/location.h>
#include <weechess/piece.h>

namespace weechess {

enum class CastleSide {
    Kingside,
    Queenside,
};

class GameState;
struct MoveHash;

class Move {
public:
    using Data = std::bitset<32>;

    Move() = default;
    Move(Data data);

    Piece moving_piece() const;
    Piece resulting_piece() const;

    static Move by_moving(Piece, Location from, Location to);
    static Move by_capturing(Piece, Location from, Location to, Piece::Type captured);
    static Move by_promoting(Piece, Location from, Location to, Piece::Type promoted);
    static Move by_castling(Piece, CastleSide);
    static Move by_en_passant(Piece, Location from, Location to);

    Location start_location() const;
    Location end_location() const;

    bool is_capture() const;
    bool is_promotion() const;
    bool is_castle() const;
    bool is_en_passant() const;
    bool is_double_pawn() const;

    Color color() const;

    void set_color(Color color);
    void set_piece_type(Piece::Type type);
    void set_origin(Location location);
    void set_destination(Location location);
    void set_capture(Piece::Type type);
    void set_promotion(Piece::Type type);
    void set_double_pawn_push();

    Piece::Type captured_piece_type() const;
    Piece::Type promoted_piece_type() const;
    std::optional<CastleSide> castle_side() const;

    std::string san_notation(const GameState&) const;

    friend bool operator==(const Move&, const Move&);
    friend bool operator!=(const Move&, const Move&);
    friend bool operator<(const Move&, const Move&);

    friend struct MoveHash;

private:
    enum class Flags : uint32_t {
        PieceType,
        Origin,
        Destination,
        Capture,
        QueensideCastle,
        KingsideCastle,
        EnPassant,
        DoublePawn,
        Promotion,
        Color,
    };

    static constexpr std::array<uint32_t, 10> masks = {
        0b00000000000000000000000000001111,
        0b00000000000000000000001111110000,
        0b00000000000000001111110000000000,
        0b00000000000011110000000000000000,
        0b00000000000100000000000000000000,
        0b00000000001000000000000000000000,
        0b00000000010000000000000000000000,
        0b00000000100000000000000000000000,
        0b00001111000000000000000000000000,
        0b00010000000000000000000000000000,
    };

    static constexpr std::array<uint32_t, 10> shifts = {
        0,
        4,
        10,
        16,
        20,
        21,
        22,
        23,
        24,
        28,
    };

    uint32_t get_flags(Flags flags) const;

    void set_flags(Flags flags, uint32_t value);

    Data m_data;
};

struct MoveHash {
    std::size_t operator()(const Move& move) const;
};

bool operator==(const Move& lhs, const Move& rhs);
bool operator!=(const Move& lhs, const Move& rhs);
bool operator<(const Move& lhs, const Move& rhs);

}
