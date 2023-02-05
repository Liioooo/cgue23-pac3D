#include <GLFW/glfw3.h>

int main(int argc, char **argv) {
    glfwInit();
    glfwCreateWindow(100, 100, "test", nullptr, nullptr);

    int i = 1;
    return 0;
}
