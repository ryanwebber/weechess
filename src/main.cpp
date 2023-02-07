#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>

#include "application/app_controller.h"
#include "game_state.h"

int main(int argc, char *argv[]) {
    auto gamestate = weechess::GameState::new_game();
    
    AppController controller;
    controller.update_state([&](auto &state) {
        state.board = gamestate.board();
        return true;
    });

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    screen.Loop(controller.renderer());
}
