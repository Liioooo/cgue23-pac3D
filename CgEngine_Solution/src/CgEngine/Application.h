#pragma once

#include <INIReader.h>
#include <Rendering/SceneRenderer.h>
#include "Window.h"
#include "Events/Event.h"
#include "Events/WindowCloseEvent.h"
#include "Events/WindowResizeEvent.h"
#include "GlobalObjectManager.h"
#include "Scene/SceneManager.h"

namespace CgEngine {

    class Application {
    public:
        explicit Application(const std::string& settingsIni);
        ~Application();

        static Application& get();

        void init();
        void run();
        void onEvent(Event& event);
        Window& getWindow();

        template<typename S>
        void registerNativeScript(const std::string& name) {
            GlobalObjectManager::getInstance().getScriptManager().registerNativeScript<S>(name);
        }

    private:
        INIReader* iniReader;
        SceneManager* sceneManager;
        SceneRenderer* sceneRenderer;
        Window* window;
        bool isRunning = true;

        void onWindowClose(WindowCloseEvent& event);
        void onWindowResize(WindowResizeEvent& event);

        static Application* instance;
    };

}

