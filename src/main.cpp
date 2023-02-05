#include <iostream>

#include "board.h"

using namespace weechess;

int main(int argc, char *argv[]) {
    auto board = Board::default_board();
    std::cout << board.to_fen() << std::endl;

    return 0;
}
