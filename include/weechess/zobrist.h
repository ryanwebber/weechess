#pragma once

#include <array>
#include <cstdint>

#include <weechess/color_map.h>

namespace weechess {

struct GameSnapshot;

namespace zobrist {

    using Hash = uint_least64_t;

    class Hasher {
    public:
        constexpr Hasher(ColorMap<Hash> turn_hash, std::array<std::array<Hash, 14>, 64> layout_hashes)
            : m_turn_hash(turn_hash)
            , m_layout_hashes(layout_hashes)
        {
        }

        Hash hash(const GameSnapshot&) const;
        static const Hasher default_instance;

    private:
        ColorMap<Hash> m_turn_hash;
        std::array<std::array<Hash, 14>, 64> m_layout_hashes;
    };
}

}
