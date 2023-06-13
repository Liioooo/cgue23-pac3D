#include "CgEngine/Logging.h"
#include "CgEngine/Application.h"

#include "Game/FlyingCameraScript.h"
#include "Game/GhostsController.h"
#include "Game/PlayerScript.h"
#include "Game/ProjectileScript.h"
#include "Game/EndScript.h"
#include "Game/StartScript.h"
#include "Game/FPSCounter.h"

int main(int argc, char **argv) {
    CgEngine::Logging::init();

    CG_LOGGING_INFO("Starting Application");
    CgEngine::Application* application = new CgEngine::Application("assets/settings.ini");

    application->registerNativeScript<Game::FlyingCameraScript>("flyingCameraScript");
    application->registerNativeScript<Game::GhostsController>("ghostsController");
    application->registerNativeScript<Game::PlayerScript>("player");
    application->registerNativeScript<Game::ProjectileScript>("projectileScript");
    application->registerNativeScript<Game::EndScript>("endScript");
    application->registerNativeScript<Game::StartScript>("startScript");
    application->registerNativeScript<Game::FPSCounter>("fpsCounter");

    application->init();
    application->run();

    delete application;

    CG_LOGGING_INFO("Shutting Down!");
    system("pause");
    return 0;
}
