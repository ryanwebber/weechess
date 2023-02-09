#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <weechess/game_state.h>

namespace weechess {
    namespace fen {
        std::string to_fen(const GameState&);
        std::optional<GameState> from_fen(std::string_view);

        constexpr std::string_view initial_gamestate_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }
}
