#include <iostream>

#include "game_state.h"

using namespace weechess;

int main(int argc, char *argv[]) {
    auto gamestate = GameState::new_game();
    std::cout << gamestate.to_fen() << std::endl;

    return 0;
}
