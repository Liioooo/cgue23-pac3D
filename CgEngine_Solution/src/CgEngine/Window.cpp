#include "Window.h"
#include "Logging.h"
#include "Events/KeyPressedEvent.h"
#include "Events/KeyReleasedEvent.h"
#include "Events/MouseButtonPressedEvent.h"
#include "Events/MouseButtonReleasedEvent.h"
#include "Events/MouseMovedEvent.h"
#include "Events/MouseScrolledEvent.h"
#include "Events/WindowCloseEvent.h"
#include "Events/WindowResizeEvent.h"

namespace CgEngine {
    Window::Window(int width, int height, bool fullScreen, int refreshRate, const std::string& title, std::function<void(Event&)>&& eventCallback) : eventCallback(std::move(eventCallback)) {
        if (!glfwInit()) {
            CG_LOGGING_ERROR("Failed to init GLFW");
        }

        glfwSetErrorCallback(&Window::errorCallback);

        #if _DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        #endif

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);

        GLFWmonitor* monitor = nullptr;
        if (fullScreen) {
            monitor = glfwGetPrimaryMonitor();
        }

        window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
        if (!window) {
            CG_LOGGING_ERROR("Failed to create Window");
        }

        glfwMakeContextCurrent(window);
        setVsync(true);

        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

        #if _DEBUG
            glDebugMessageCallback(&Window::debugCallback, nullptr);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        #endif

        glfwSetWindowUserPointer(window, this);

        glfwSetWindowCloseCallback(window, [](GLFWwindow* w) {
            WindowCloseEvent e;
            static_cast<Window*>(glfwGetWindowUserPointer(w))->eventCallback(e);
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) {
            WindowResizeEvent e(width, height);
            static_cast<Window*>(glfwGetWindowUserPointer(w))->eventCallback(e);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent e(button);
                    static_cast<Window *>(glfwGetWindowUserPointer(w))->eventCallback(e);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent e(button);
                    static_cast<Window *>(glfwGetWindowUserPointer(w))->eventCallback(e);
                    break;
                }
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
            MouseMovedEvent e(static_cast<float>(x), static_cast<float>(y));
            static_cast<Window*>(glfwGetWindowUserPointer(w))->eventCallback(e);
        });

        glfwSetScrollCallback(window, [](GLFWwindow* w, double offsetX, double offsetY) {
            MouseScrolledEvent e(static_cast<float>(offsetX), static_cast<float>(offsetY));
            static_cast<Window*>(glfwGetWindowUserPointer(w))->eventCallback(e);
        });

        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent e(key, mods);
                    static_cast<Window *>(glfwGetWindowUserPointer(w))->eventCallback(e);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent e(key, mods);
                    static_cast<Window *>(glfwGetWindowUserPointer(w))->eventCallback(e);
                    break;
                }
            }
        });
    }

    Window::~Window() {
        glfwDestroyWindow(window);
    }

    void Window::setVsync(bool enabled) {
        vsync = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    bool Window::isVsync() {
        return vsync;
    }

    void Window::onUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    void Window::errorCallback(int error, const char *description) {
        CG_LOGGING_ERROR("GlFW ERROR {0}: {1}", error, description);
    }

    void Window::debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, const void* userParam) {
        if (id == 131185 || id == 131218) {
            return; // ignore performance warnings (buffer uses GPU memory, shader recompilation) from nvidia
        }

        std::stringstream stringStream;
        std::string sourceString;
        std::string typeString;
        std::string severityString;

        switch (source) {
            case GL_DEBUG_SOURCE_API: {
                sourceString = "API";
                break;
            }
            case GL_DEBUG_SOURCE_APPLICATION: {
                sourceString = "Application";
                break;
            }
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
                sourceString = "Window System";
                break;
            }
            case GL_DEBUG_SOURCE_SHADER_COMPILER: {
                sourceString = "Shader Compiler";
                break;
            }
            case GL_DEBUG_SOURCE_THIRD_PARTY: {
                sourceString = "Third Party";
                break;
            }
            case GL_DEBUG_SOURCE_OTHER: {
                sourceString = "Other";
                break;
            }
            default: {
                sourceString = "Unknown";
                break;
            }
        }

        switch (type) {
            case GL_DEBUG_TYPE_ERROR: {
                typeString = "Error";
                break;
            }
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
                typeString = "Deprecated Behavior";
                break;
            }
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
                typeString = "Undefined Behavior";
                break;
            }
            case GL_DEBUG_TYPE_PERFORMANCE: {
                typeString = "Performance";
                break;
            }
            case GL_DEBUG_TYPE_OTHER: {
                typeString = "Other";
                break;
            }
            default: {
                typeString = "Unknown";
                break;
            }
        }

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH: {
                severityString = "High";
                break;
            }
            case GL_DEBUG_SEVERITY_MEDIUM: {
                severityString = "Medium";
                break;
            }
            case GL_DEBUG_SEVERITY_LOW: {
                severityString = "Low";
                break;
            }
            default: {
                severityString = "Unknown";
                break;
            }
        }

        stringStream << "OpenGL Error: " << message;
        stringStream << " [Source = " << sourceString;
        stringStream << ", Type = " << typeString;
        stringStream << ", Severity = " << severityString;
        stringStream << ", ID = " << id << "]";

        CG_LOGGING_WARNING(stringStream.str());
    }

}
