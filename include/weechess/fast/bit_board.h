#pragma once

#include <bitset>
#include <cassert>
#include <iostream>
#include <span>
#include <tuple>
#include <weechess/location.h>

namespace weechess::fast {

class BitBoard {
public:
    // std::bitset::set() is not constexpr in stdlib until c++23
    using Data = uint64_t;

    constexpr BitBoard()
        : m_data(0)
    {
    }

    constexpr BitBoard(Data data)
        : m_data(data) {};

    constexpr void set(Location location) { m_data = m_data | (1ull << location.offset); }

    constexpr void unset(Location location) { m_data = m_data & ~(1ull << location.offset); }

    constexpr bool operator[](Location location) const { return (m_data & (1ull << location.offset)) != 0ull; }

    constexpr BitBoard operator|(const BitBoard& other) const { return BitBoard(m_data | other.m_data); }
    constexpr BitBoard operator&(const BitBoard& other) const { return BitBoard(m_data & other.m_data); }
    constexpr BitBoard operator~() const { return BitBoard(~m_data); }
    constexpr void operator|=(const BitBoard& other) { m_data |= other.m_data; }
    constexpr void operator&=(const BitBoard& other) { m_data &= other.m_data; }

    constexpr bool all() const { return m_data == ~0ull; }
    constexpr bool any() const { return m_data != 0; }
    constexpr bool none() const { return m_data == 0; }

    constexpr Data data() const { return m_data; }

    constexpr std::optional<Location> msb() const
    {
        if (none())
            return {};

        return 63 - __builtin_clzll(m_data);
    }
    constexpr std::optional<Location> lsb() const
    {
        if (none())
            return {};

        return __builtin_ffsll(m_data) - 1;
    }

    constexpr std::optional<Location> pop_lsb()
    {
        if (auto lsb = this->lsb()) {
            m_data &= m_data - 1;
            return *lsb;
        }

        return {};
    }

    constexpr uint8_t count_set_bits() const { return __builtin_popcountll(m_data); }

    static constexpr BitBoard from(std::span<const Location> locations)
    {
        BitBoard bb;
        for (const auto& location : locations) {
            bb.set(location.offset);
        }

        return bb;
    }

    static constexpr BitBoard empty() { return BitBoard(); }

private:
    Data m_data;
};

constexpr bool operator==(const BitBoard& lhs, const BitBoard& rhs) { return lhs.data() == rhs.data(); }
constexpr bool operator!=(const BitBoard& lhs, const BitBoard& rhs) { return lhs.data() != rhs.data(); }

std::ostream& operator<<(std::ostream&, const BitBoard&);
}
