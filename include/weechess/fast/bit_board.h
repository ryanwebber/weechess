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
    constexpr void operator|=(const BitBoard& other) { m_data |= other.m_data; }
    constexpr void operator&=(const BitBoard& other) { m_data &= other.m_data; }

    constexpr bool all() const { return m_data.all(); }
    constexpr bool any() const { return m_data.any(); }
    constexpr bool none() const { return m_data.none(); }

    constexpr Data data() const { return m_data; }

    constexpr std::optional<Location> msb() const
    {
        if (m_data.none())
            return {};

        return 63 - __builtin_clzll(m_data.to_ullong());
    }
    constexpr std::optional<Location> lsb() const
    {
        if (m_data.none())
            return {};

        return __builtin_ffsll(m_data.to_ulong()) - 1;
    }

    constexpr std::optional<Location> pop_lsb()
    {
        if (auto lsb = this->lsb()) {
            m_data &= m_data.to_ullong() - 1;
            return *lsb;
        }

        return {};
    }

    constexpr uint8_t count_set_bits() const { return __builtin_popcountll(m_data.to_ullong()); }

    static constexpr BitBoard from(std::span<const Location> locations)
    {
        Data data = 0;
        for (const auto& location : locations) {
            data.set(location.offset);
        }

        return BitBoard(data);
    }

    static constexpr BitBoard empty() { return BitBoard(); }

private:
    Data m_data;
};

constexpr bool operator==(const BitBoard& lhs, const BitBoard& rhs) { return lhs.data() == rhs.data(); }
constexpr bool operator!=(const BitBoard& lhs, const BitBoard& rhs) { return lhs.data() != rhs.data(); }

std::ostream& operator<<(std::ostream&, const BitBoard&);
}
