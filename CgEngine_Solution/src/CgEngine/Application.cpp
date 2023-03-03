#include <Rendering/Renderer.h>
#include "Application.h"
#include "Logging.h"
#include "FileSystem.h"

namespace CgEngine {
    Application* Application::instance = nullptr;

    Application::Application(const std::string &settingsIni) {
        try {
            iniReader = new INIReader(settingsIni);
        } catch (const std::runtime_error& e) {
            CG_LOGGING_ERROR(e.what())
        }
        Application::instance = this;
    }

    Application::~Application() {
        Renderer::shutdown();
        delete iniReader;
        delete window;
    }

    Application &Application::get() {
        return *Application::instance;
    }

    void Application::init() {
        WindowSpecification windowSpecification;
        windowSpecification.width = iniReader->GetInteger("window", "width", 1280);
        windowSpecification.width = iniReader->GetInteger("window", "width", 720);
        windowSpecification.title = iniReader->Get("window", "title", "CG Engine");
        windowSpecification.fullScreen = iniReader->GetBoolean("window", "fullscreen", false);
        windowSpecification.refreshRate = iniReader->GetInteger("window", "refresh_rate", 60);
        windowSpecification.vSync = iniReader->GetBoolean("window", "v_sync", true);

        window = new Window(windowSpecification, EVENT_BIND_FN(onEvent));

        sceneManager = &GlobalObjectManager::getInstance().getSceneManager();
        sceneManager->setViewportSize(window->getWidth(), window->getHeight());
        sceneManager->setActiveScene(FileSystem::getAsGamePath(iniReader->Get("game", "startScene", "default_scene.xml")));
        sceneRenderer = new SceneRenderer(window->getWidth(), window->getHeight());
    }

    void Application::run() {
        while (isRunning) {
            window->pollEvents();

            Scene& activeScene = *sceneManager->getActiveScene();
            activeScene.onUpdate(timeStep);
            activeScene.onRender(*sceneRenderer);

            window->swapBuffers();

            float time = getTime();
            timeStep = time - lastFrameTime;
            lastFrameTime = time;
        }
    }

    void Application::onEvent(Event& event) {
        EventDispatcher eventDispatcher(event);

        eventDispatcher.dispatch<WindowCloseEvent>(EVENT_BIND_FN(onWindowClose));
        eventDispatcher.dispatch<WindowResizeEvent>(EVENT_BIND_FN(onWindowResize));
    }

    Window &Application::getWindow() {
        return *window;
    }

    float Application::getTime() {
        return static_cast<float>(glfwGetTime());
    }

    void Application::onWindowClose(WindowCloseEvent &event) {
        isRunning = false;
    }

    void Application::onWindowResize(WindowResizeEvent &event) {
        sceneManager->setViewportSize(event.getWidth(), event.getHeight());
        sceneRenderer->setViewportSize(event.getWidth(), event.getHeight());
    }
}
