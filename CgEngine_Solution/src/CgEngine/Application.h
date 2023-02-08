#pragma once

#include <INIReader.h>
#include "Window.h"
#include "Events/Event.h"
#include "Events/WindowCloseEvent.h"

namespace CgEngine {

    class Application {
    public:
        explicit Application(const std::string& settingsIni);
        ~Application();

        void run();
        void onEvent(Event& event);

    private:
        INIReader* iniReader;
        Window* window;
        bool isRunning = true;

        void onWindowClose(WindowCloseEvent& event);
    };

}

