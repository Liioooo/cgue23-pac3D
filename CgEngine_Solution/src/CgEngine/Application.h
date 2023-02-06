#pragma once

#include <INIReader.h>
#include "Window.h"

namespace CgEngine {

    class Application {
    public:
        explicit Application(const std::string& settingsIni);
        ~Application();

        void run();

    private:
        INIReader* iniReader;
        Window* window;
        bool isRunning = true;
    };

}

