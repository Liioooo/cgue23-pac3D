#include "CgEngine/Logging.h"
#include "CgEngine/Application.h"

#include "Game/FlyingCameraScript.h"
#include "Game/GhostsController.h"
#include "Game/PlayerScript.h"

int main(int argc, char **argv) {
    CgEngine::Logging::init();

    CG_LOGGING_INFO("Starting Application");
    CgEngine::Application* application = new CgEngine::Application("assets/settings.ini");

    application->registerNativeScript<Game::FlyingCameraScript>("flyingCameraScript");
    application->registerNativeScript<Game::GhostsController>("ghostsController");
    application->registerNativeScript<Game::PlayerScript>("player");

    application->init();
    application->run();

    delete application;

    CG_LOGGING_INFO("Shutting Down!");
    return 0;
}
