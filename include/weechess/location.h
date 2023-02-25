#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <string>

namespace weechess {

struct Location {

    struct FileShift {
        int value;

        constexpr FileShift operator*(uint8_t n) const { return FileShift { value * n }; }
    };

    struct RankShift {
        int value;

        constexpr RankShift operator*(uint8_t n) const { return RankShift { value * n }; }
    };

    constexpr static FileShift Left = FileShift { -1 };
    constexpr static FileShift Right = FileShift { 1 };
    constexpr static RankShift Up = RankShift { 1 };
    constexpr static RankShift Down = RankShift { -1 };

    uint8_t offset;

    constexpr Location(uint8_t offset)
        : offset(offset) {};

    bool is_valid() const;

    constexpr uint8_t file() const { return offset % 8; }
    constexpr uint8_t rank() const { return offset / 8; }

    std::string to_string() const;

    constexpr std::optional<Location> offset_by(int8_t offset) const
    {
        if (offset >= 64) {
            return {};
        }

        return Location(offset);
    }

    constexpr std::optional<Location> offset_by(FileShift fs, RankShift rs = {}) const
    {
        auto file = fs.value + this->file();
        auto rank = rs.value + this->rank();

        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            return {};
        }

        return Location::from_rank_and_file(rank, file);
    }

    constexpr std::optional<Location> offset_by(RankShift rs = {}, FileShift fs = {}) const
    {
        return this->offset_by(fs, rs);
    }

    template <typename F> void with_offset(FileShift fs = {}, RankShift rs = {}, F f = {}) const
    {
        if (auto location = this->offset_by(fs, rs)) {
            f(*location);
        }
    }

    static constexpr Location from_rank_and_file(uint8_t rank, uint8_t file) { return Location(rank, file); }
    static std::optional<Location> from_string(std::string_view);

    friend bool operator==(Location const&, Location const&) = default;

    // All 64 individual locations with named variables
    static const Location A1;
    static const Location B1;
    static const Location C1;
    static const Location D1;
    static const Location E1;
    static const Location F1;
    static const Location G1;
    static const Location H1;
    static const Location A2;
    static const Location B2;
    static const Location C2;
    static const Location D2;
    static const Location E2;
    static const Location F2;
    static const Location G2;
    static const Location H2;
    static const Location A3;
    static const Location B3;
    static const Location C3;
    static const Location D3;
    static const Location E3;
    static const Location F3;
    static const Location G3;
    static const Location H3;
    static const Location A4;
    static const Location B4;
    static const Location C4;
    static const Location D4;
    static const Location E4;
    static const Location F4;
    static const Location G4;
    static const Location H4;
    static const Location A5;
    static const Location B5;
    static const Location C5;
    static const Location D5;
    static const Location E5;
    static const Location F5;
    static const Location G5;
    static const Location H5;
    static const Location A6;
    static const Location B6;
    static const Location C6;
    static const Location D6;
    static const Location E6;
    static const Location F6;
    static const Location G6;
    static const Location H6;
    static const Location A7;
    static const Location B7;
    static const Location C7;
    static const Location D7;
    static const Location E7;
    static const Location F7;
    static const Location G7;
    static const Location H7;
    static const Location A8;
    static const Location B8;
    static const Location C8;
    static const Location D8;
    static const Location E8;
    static const Location F8;
    static const Location G8;
    static const Location H8;

private:
    constexpr Location(uint8_t rank, uint8_t file)
        : offset(file + rank * 8)
    {
    }
};

std::ostream& operator<<(std::ostream&, const Location&);

}
