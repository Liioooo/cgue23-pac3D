#include "Logging.h"

namespace CgEngine {
    std::shared_ptr<spdlog::logger> Logging::cgLogger;

    void Logging::init() {
        cgLogger = spdlog::stdout_color_mt("CG");
        cgLogger->set_pattern("[%H:%M:%S][%n][%l]: %v");
    }
}
