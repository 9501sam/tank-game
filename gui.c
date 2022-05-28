#include "glad.h"
// sudo apt-get install libglfw3 libglfw3-dev
#include <GLFW/glfw3.h>

#include "tankio.h"

static GLFWwindow *window;

static void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void init_ui(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return;
    }
    glViewport(0, 0, 800, 600);
}

void deinit_ui(void) {
    glfwTerminate();
}

void erase_tank(const tank *tk) {
}

void print_tank_info(const tank *tk) {
}

void erase_tank_info(const tank *tk) {
}

void print_tank(const tank *tk) {
}

void erase_bullet(const bullet *blt) {
}

void print_bullet(const bullet *blt) {
}

void print_all_walls(void) {
}

void refresh_screen(void) {
}

input_t get_input(void) {
    if (glfwWindowShouldClose(window))
        exit(EXIT_SUCCESS);
    processInput(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
    return INPUT_INVALID;
}

