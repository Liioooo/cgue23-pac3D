#include "Application.h"
#include "Logging.h"
#include "Events/MouseMovedEvent.h"

namespace CgEngine {
    Application::Application(const std::string &settingsIni) {
        try {
            iniReader = new INIReader(settingsIni);
        } catch (const std::runtime_error& e) {
            CG_LOGGING_ERROR(e.what())
        }

        window = new Window(
                iniReader->GetInteger("window", "width", 1280),
                iniReader->GetInteger("window", "width", 720),
                iniReader->GetBoolean("window", "fullscreen", false),
                iniReader->GetInteger("window", "refresh_rate", 60),
                iniReader->Get("window", "title", "CG Engine"),
                EVENT_BIND_FN(onEvent)
                );
    }

    Application::~Application() {
        delete iniReader;
        delete window;
    }

    void Application::run() {
        while (isRunning) {
            window->onUpdate();
        }
    }

    void Application::onEvent(Event& event) {
        EventDispatcher eventDispatcher(event);

        eventDispatcher.dispatch<WindowCloseEvent>(EVENT_BIND_FN(onWindowClose));
    }

    void Application::onWindowClose(WindowCloseEvent &event) {
        isRunning = false;
    }
}
