#pragma once

#include <optional>
#include <span>
#include <vector>

#include "board.h"
#include "location.h"

struct BoardRender {
    enum Decoration {
        None,
        Selected,
        Highlighted,
    };

    struct Cell {
        char16_t symbol;
        Decoration decoration;
    };

    std::vector<std::vector<Cell>> cells;
    size_t width;
    size_t height;
};

class BoardPrinter {
public:
    BoardPrinter() = default;
    BoardRender print(
        const weechess::Board& board,
        std::optional<weechess::Location> selected_location,
        std::span<const weechess::Location> highlighted_locations) const;
};
