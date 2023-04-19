#pragma once

#include <INIReader.h>
#include <Rendering/SceneRenderer.h>
#include "Window.h"
#include "Events/Event.h"
#include "Events/WindowCloseEvent.h"
#include "Events/WindowResizeEvent.h"
#include "Events/KeyPressedEvent.h"
#include "GlobalObjectManager.h"
#include "Scene/SceneManager.h"
#include "TimeStep.h"

namespace CgEngine {

    struct ApplicationOptions {
        bool debugShowPhysicsColliders;
        bool debugShowNormals;
        bool debugRenderLines;

        float anisotropicFiltering;
        uint32_t shadowMapResolution;
    };

    class Application {
    public:
        explicit Application(const std::string& settingsIni);
        ~Application();

        static Application& get();

        void init();
        void run();
        void onEvent(Event& event);
        Window& getWindow();
        float getTime();
        ApplicationOptions& getApplicationOptions();

        template<typename S>
        void registerNativeScript(const std::string& name) {
            GlobalObjectManager::getInstance().getScriptManager().registerNativeScript<S>(name);
        }

    private:
        INIReader* iniReader;
        ApplicationOptions applicationOptions;
        SceneManager* sceneManager;
        SceneRenderer* sceneRenderer;
        Window* window;
        bool isRunning = true;
        float lastFrameTime = getTime();
        TimeStep timeStep{};

        void onWindowClose(WindowCloseEvent& event);
        void onWindowResize(WindowResizeEvent& event);
        void onKeyPressed(KeyPressedEvent& event);

        static Application* instance;
    };

}

