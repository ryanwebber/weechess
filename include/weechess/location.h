#pragma once

#include <functional>
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
    constexpr static RankShift Up = RankShift { -1 };
    constexpr static RankShift Down = RankShift { 1 };

    uint8_t offset;

    Location(uint8_t offset);

    bool is_valid() const;

    uint8_t file() const;
    uint8_t rank() const;

    std::string to_string() const;

    std::optional<Location> offset_by(int8_t offset) const;
    std::optional<Location> offset_by(FileShift = {}, RankShift = {}) const;
    std::optional<Location> offset_by(RankShift rs = {}, FileShift fs = {}) const { return this->offset_by(fs, rs); }

    template <typename F> void with_offset(FileShift fs = {}, RankShift rs = {}, F f = {}) const
    {
        if (auto location = this->offset_by(fs, rs)) {
            f(*location);
        }
    }

    Location chromatic_inverse() const;

    static Location from_rank_and_file(uint8_t rank, uint8_t file);
    static std::optional<Location> from_name(std::string_view);

    friend bool operator==(Location const&, Location const&) = default;

private:
    Location(uint8_t rank, uint8_t file);
};

}
