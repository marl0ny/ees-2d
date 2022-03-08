#include "gl_wrappers2d/gl_wrappers.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <utility>
#include <cmath>

#pragma once

struct MouseClick {
    double x, y;
    double dx, dy;
    bool pressed = false;
    bool released = false;
    int w, h;
    int mouse_button;
    MouseClick(int mb) {
        mouse_button = mb;
    }
    void update(GLFWwindow *window) {
        double x_prev = x;
        double y_prev = y;
        glfwGetFramebufferSize(window, &w, &h);
        glfwGetCursorPos(window, &x, &y);
        x = x/(double)w;
        y = 1.0 - y/(double)h;
        dx = x - x_prev;
        dy = y - y_prev;
        if (glfwGetMouseButton(window, mouse_button) == GLFW_PRESS) {
            pressed = true;
        } else {
            if (released) released = false;
            if (pressed) released = true;
            pressed = false;
        }
    }
};


void button_update(GLFWwindow *window, int button_key, 
                    int &param, int new_val);