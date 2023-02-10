#pragma once

#include <spdlog/spdlog.h>

namespace log {

    constexpr const char* subsystem = "cli";

    void init_logging();

    template<typename... Args>
    inline void debug(spdlog::format_string_t<Args...> fmt, Args &&... args) {
        spdlog::get(subsystem)->template debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void warn(spdlog::format_string_t<Args...> fmt, Args &&... args) {
        spdlog::get(subsystem)->template warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void error(spdlog::format_string_t<Args...> fmt, Args &&... args) {
        spdlog::get(subsystem)->template error(fmt, std::forward<Args>(args)...);
    }
}
