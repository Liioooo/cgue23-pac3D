#include <GLFW/glfw3.h>
#include <glad/glad.h>


int main(int argc, char **argv) {
    glfwInit();
    auto* win = glfwCreateWindow(400, 400, "test", nullptr, nullptr);

    glfwMakeContextCurrent(win);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glClearColor(1, 0, 1, 1);

    glm::vec3 v = glm::vec3(1,1,1);
    std::vector<int> vec{};


    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(win);
    }


    return 0;
}
