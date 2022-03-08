#include "input_handling.hpp"

void button_update(GLFWwindow *window, int button_key, 
                    int &param, int new_val) {
    if (glfwGetKey(window, button_key) == GLFW_PRESS) {
        param = new_val;
    }
}