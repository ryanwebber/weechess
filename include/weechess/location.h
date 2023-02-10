#pragma once

#include <optional>
#include <string>

namespace weechess {

struct Location {
    uint8_t offset;

    Location(uint8_t offset);

    uint8_t file() const;
    uint8_t rank() const;

    std::string to_string() const;

    std::optional<Location> offset_by(int8_t offset);
    std::optional<Location> offset_by(int8_t file_offset, int8_t rank_offset);

    static Location from_rank_and_file(uint8_t rank, uint8_t file);
    static std::optional<Location> from_name(std::string_view);

    friend bool operator==(Location const&, Location const&) = default;

private:
    Location(uint8_t rank, uint8_t file);
};

}
