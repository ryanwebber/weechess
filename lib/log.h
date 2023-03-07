#pragma once

#include <iostream>

#if WEECHESS_LOGGING_ENABLED
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#endif

namespace weechess::log {

constexpr const char* subsystem = "weechess";

#if WEECHESS_LOGGING_ENABLED
template <typename... Args> inline void debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    spdlog::get(subsystem)->template debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args> inline void warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    spdlog::get(subsystem)->template warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args> inline void error(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    spdlog::get(subsystem)->template error(fmt, std::forward<Args>(args)...);
}
#else
template <typename... Args> inline void debug(std::string_view fmt, Args&&... args) { }

template <typename... Args> inline void warn(std::string_view fmt, Args&&... args) { }

template <typename... Args> inline void error(std::string_view fmt, Args&&... args) { }
#endif
}
