#pragma once

#include <bitset>
#include <cassert>
#include <span>
#include <tuple>
#include <weechess/location.h>

namespace weechess {

class BitBoard {
private:
    // clang-format off
    const uint64_t debruijn64index[64] = {
        63,  0, 58,  1, 59, 47, 53,  2,
        60, 39, 48, 27, 54, 33, 42,  3,
        61, 51, 37, 40, 49, 18, 28, 20,
        55, 30, 34, 11, 43, 14, 22,  4,
        62, 57, 46, 52, 38, 26, 32, 41,
        50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16,  9, 12,
        44, 24, 15,  8, 23,  7,  6,  5
    };
    // clang-format on
public:
    using Data = std::bitset<64>;

    constexpr BitBoard()
        : m_data(0)
    {
    }

    constexpr BitBoard(Data data)
        : m_data(data) {};

    constexpr void set(Location location) { m_data.set(location.offset); }
    constexpr void reset(Location location) { m_data.reset(location.offset); }
    constexpr bool operator[](Location location) const { return m_data[location.offset]; }

    constexpr BitBoard operator|(const BitBoard& other) const { return BitBoard(m_data | other.m_data); }
    constexpr BitBoard operator&(const BitBoard& other) const { return BitBoard(m_data & other.m_data); }
    constexpr BitBoard operator~() const { return BitBoard(~m_data); }

    constexpr bool all() const { return m_data.all(); }
    constexpr bool any() const { return m_data.any(); }
    constexpr bool none() const { return m_data.none(); }

    constexpr Data data() const { return m_data; }

    constexpr Location lsb() const
    {
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        constexpr uint64_t magic = 0x07EDD5E59A4E28C2ULL;
        uint64_t value = m_data.to_ullong();
        uint8_t offset = debruijn64index[((value & -value) * magic) >> 58];
        return Location(offset);
    }

    static constexpr BitBoard from(std::span<const Location> locations)
    {
        Data data = 0;
        for (const auto& location : locations) {
            data.set(location.offset);
        }

        return BitBoard(data);
    }

private:
    Data m_data;
};

}
