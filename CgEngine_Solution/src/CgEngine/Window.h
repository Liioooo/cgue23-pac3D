#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace CgEngine {

    class Window {
    public:
        Window(int width, int height, bool fullScreen, int refreshRate, const std::string& title);
        ~Window();

        void setVsync(bool enabled);
        inline bool isVsync();
        void onUpdate();

    private:
        bool vsync = false;
        GLFWwindow* window;

        static void errorCallback(int error, const char* description);
        static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);
    };

}
