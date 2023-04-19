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
        applicationOptions.debugShowPhysicsColliders = iniReader->GetBoolean("application", "debug_show_physics_colliders", false);
        applicationOptions.debugShowNormals = iniReader->GetBoolean("application", "debug_show_normals", false);
        applicationOptions.debugRenderLines = iniReader->GetBoolean("application", "debug_render_lines", false);
        applicationOptions.anisotropicFiltering = static_cast<float>(iniReader->GetReal("application", "anisotropic_filtering", 1.0));
        applicationOptions.shadowMapResolution = iniReader->GetInteger("application", "shadow_map_resolution", 2048);

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
        lastFrameTime = getTime();

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
        eventDispatcher.dispatch<KeyPressedEvent>(EVENT_BIND_FN(onKeyPressed));
    }

    Window &Application::getWindow() {
        return *window;
    }

    float Application::getTime() {
        return static_cast<float>(glfwGetTime());
    }

    ApplicationOptions& Application::getApplicationOptions() {
        return applicationOptions;
    }

    void Application::onWindowClose(WindowCloseEvent &event) {
        isRunning = false;
    }

    void Application::onWindowResize(WindowResizeEvent &event) {
        sceneManager->setViewportSize(event.getWidth(), event.getHeight());
        sceneRenderer->setViewportSize(event.getWidth(), event.getHeight());
    }

    void Application::onKeyPressed(KeyPressedEvent& event) {
        switch (event.getKeyCode()) {
            case KeyCode::F1: {
                applicationOptions.debugShowPhysicsColliders = !applicationOptions.debugShowPhysicsColliders;
                break;
            }
            case KeyCode::F2: {
                applicationOptions.debugShowNormals = !applicationOptions.debugShowNormals;
                break;
            }
        }
    }
}
