#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <weechess/game_state.h>

namespace weechess {
namespace fen {
    std::string to_fen(const GameSnapshot&);
    std::optional<GameSnapshot> from_fen(std::string_view);

    constexpr std::string_view initial_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}
}
