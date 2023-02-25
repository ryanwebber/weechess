#include <catch2/catch_session.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class TestRunListener : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testRunStarting(Catch::TestRunInfo const&) override
    {
        auto lib_logger = spdlog::stdout_color_mt("weechess");
        lib_logger->set_level(spdlog::level::debug);

        if (!spdlog::get("weechess")) {
            spdlog::register_logger(lib_logger);
        }
    }

    void testRunEnded(Catch::TestRunStats const&) override { spdlog::drop_all(); }
};

CATCH_REGISTER_LISTENER(TestRunListener)

int main(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);
    return result;
}
