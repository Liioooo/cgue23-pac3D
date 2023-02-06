#include "CgEngine/Logging.h"
#include "CgEngine/Application.h"

int main(int argc, char **argv) {
    CgEngine::Logging::init();

    CG_LOGGING_INFO("Starting Application");
    CgEngine::Application* application = new CgEngine::Application("assets/settings.ini");

    application->run();

    CG_LOGGING_INFO("Shutting Down!");
    return 0;
}
