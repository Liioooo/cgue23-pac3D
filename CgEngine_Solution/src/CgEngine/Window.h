#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Events/Event.h"

namespace CgEngine {

    struct WindowSpecification {
        std::string title;
        uint32_t width = 1280;
        uint32_t height = 720;
        uint32_t refreshRate = 60;
        bool fullScreen = false;
        bool vSync = true;
    };

    class Window {
    public:
        Window(const WindowSpecification& spec, std::function<void(Event&)>&& eventCallback);
        ~Window();

        void setVsync(bool enabled);
        inline bool isVsync();
        void onUpdate();
        uint32_t getWidth();
        uint32_t getHeight();
        GLFWwindow& getWindowHandle();

    private:
        bool vsync = false;
        uint32_t windowWidth;
        uint32_t windowHeight;
        GLFWwindow* window;
        const std::function<void(Event&)> eventCallback;

        static void errorCallback(int error, const char* description);
        static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);
    };

}
