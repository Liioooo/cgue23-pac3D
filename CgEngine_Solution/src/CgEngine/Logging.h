#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace CgEngine {

    class Logging {
    public:
        static void init();

        inline static std::shared_ptr<spdlog::logger> getCGLogger() {
            return cgLogger;
        };
    private:
        static std::shared_ptr<spdlog::logger> cgLogger;
    };
}

#define CG_LOGGING_INFO(...) ::CgEngine::Logging::getCGLogger()->info(__VA_ARGS__);
#define CG_LOGGING_WARNING(...) ::CgEngine::Logging::getCGLogger()->warn(__VA_ARGS__);
#define CG_LOGGING_ERROR(...) ::CgEngine::Logging::getCGLogger()->error(__VA_ARGS__);