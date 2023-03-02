#include <chrono>
#include <spdlog/sinks/basic_file_sink.h>

#include "log.h"

namespace log {
void init_logging()
{
    auto shared_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(WEECHESS_LOG_FILE);

    auto lib_logger = std::make_shared<spdlog::logger>("weechess", shared_sink);
    lib_logger->set_level(spdlog::level::debug);
    spdlog::register_logger(lib_logger);

    auto cli_logger = std::make_shared<spdlog::logger>("cli", shared_sink);
    cli_logger->set_level(spdlog::level::debug);
    spdlog::register_logger(cli_logger);

    spdlog::flush_every(std::chrono::milliseconds(60));
}
}
