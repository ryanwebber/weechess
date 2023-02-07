#pragma once

#include <sstream>
#include <vector>

#include "board.h"


struct BoardRender {
    std::vector<std::string> lines;
    size_t width;
    size_t height;

    static BoardRender from(const weechess::Board& board);
};
