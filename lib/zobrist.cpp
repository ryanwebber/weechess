#include <weechess/game_state.h>
#include <weechess/zobrist.h>

namespace weechess::zobrist {
namespace {

    constexpr Hash seed = 0;
    constexpr Hash lce_a = 6364136223846793005ULL;
    constexpr Hash lce_c = 1442695040888963407ULL;
    constexpr Hash lce_m = 18446744073709551615ULL;

    constexpr static Hash next_psuedo_random(Hash& previous)
    {
        previous = ((lce_a * previous + lce_c) % lce_m);
        return previous;
    }

    constexpr Hasher generate_zobrist_hasher()
    {
        ColorMap<Hash> turn_hash {};
        std::array<std::array<Hash, 14>, 64> layout_hashes {};

        auto random_state = seed;
        turn_hash[Color::White] = next_psuedo_random(random_state);
        turn_hash[Color::Black] = next_psuedo_random(random_state);

        for (const auto& piece : Piece::all_valid_pieces) {
            auto piece_index = static_cast<int>(piece.type);
            for (int i = 0; i < 64; ++i) {
                layout_hashes[i][piece_index] = next_psuedo_random(random_state);
            }
        }

        return Hasher(turn_hash, layout_hashes);
    }
}

const Hasher Hasher::default_instance = generate_zobrist_hasher();

Hash Hasher::hash(const GameSnapshot& snapshot) const
{
    Hash hash = 0;

    for (const auto& piece : Piece::all_valid_pieces) {
        auto piece_index = static_cast<int>(piece.type);
        auto occupancy = snapshot.board.occupancy_for(piece);
        while (occupancy.any()) {
            auto square = occupancy.pop_lsb();
            hash ^= m_layout_hashes[square->offset][piece_index];
        }
    }

    hash ^= m_turn_hash[snapshot.turn_to_move];

    return hash;
}
}
