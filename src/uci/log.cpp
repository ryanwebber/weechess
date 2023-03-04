#include <chrono>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "log.h"

namespace log {
void init_logging()
{
    auto shared_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();

    auto lib_logger = std::make_shared<spdlog::logger>("weechess", shared_sink);
    lib_logger->set_level(spdlog::level::debug);
    spdlog::register_logger(lib_logger);

    auto cli_logger = std::make_shared<spdlog::logger>("cli", shared_sink);
    cli_logger->set_level(spdlog::level::debug);
    spdlog::register_logger(cli_logger);
}
}
