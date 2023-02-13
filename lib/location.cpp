#include <weechess/board.h>
#include <weechess/location.h>

namespace weechess {

Location::Location(uint8_t offset)
    : offset(offset)
{
}

Location::Location(uint8_t rank, uint8_t file)
    : offset(file + rank * 8)
{
}

bool Location::is_valid() const { return offset < Board::cell_count; }

uint8_t Location::file() const { return offset % 8; }

uint8_t Location::rank() const { return offset / 8; }

std::string Location::to_string() const { return std::string(1, 'a' + file()) + std::to_string(rank() + 1); }

std::optional<Location> Location::offset_by(int8_t offset)
{
    if (offset >= Board::cell_count) {
        return std::nullopt;
    }

    return Location(offset);
}

std::optional<Location> Location::offset_by(int8_t file_offset, int8_t rank_offset)
{
    int8_t file = file_offset + this->file();
    int8_t rank = rank_offset + this->rank();

    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return std::nullopt;
    }

    return Location::from_rank_and_file(rank, file);
}

Location Location::opposite() const { return Location(Board::cell_count - offset - 1); }

Location Location::operator+(int8_t offset) const { return Location(this->offset + offset); }
Location Location::operator-(int8_t offset) const { return Location(this->offset - offset); }

Location Location::from_rank_and_file(uint8_t rank, uint8_t file) { return Location(rank, file); }

std::optional<Location> Location::from_name(std::string_view name)
{
    if (name.size() != 2) {
        return std::nullopt;
    }

    uint8_t file = name[0] - 'a';
    uint8_t rank = name[1] - '1';

    if (file > 7 || rank > 7) {
        return std::nullopt;
    }

    return Location::from_rank_and_file(rank, file);
}
}