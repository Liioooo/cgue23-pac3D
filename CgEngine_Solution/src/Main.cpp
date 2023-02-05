#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main(int argc, char **argv) {
    glfwInit();
    auto* win = glfwCreateWindow(400, 400, "test", nullptr, nullptr);

    glfwMakeContextCurrent(win);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glClearColor(1, 0, 1, 1);


    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(win);
    }


    return 0;
}
