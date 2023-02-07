#pragma once

#include <optional>
#include <vector>

#include "board.h"


struct BoardRender {
    enum Decoration {
        None,
        Selected,
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
    BoardRender print(const weechess::Board& board, std::optional<weechess::Location> selected_location) const;
};
