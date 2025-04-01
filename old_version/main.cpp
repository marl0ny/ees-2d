#include "gl_wrappers2d/gl_wrappers.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <utility>
#include <cmath>
#include "shader_programs.hpp"
#include "params.hpp"
#include "input_handling.hpp"
#include "init_energy_states.hpp"
#include "init_potential.hpp"
#include "complex_float_rgba.hpp"
#include "to_rgba_array.hpp"
#include "draw_view.hpp"
#include "shaders.hpp"
#ifdef __EMSCRIPTEN__
#include <functional>
#include <emscripten.h>
#endif


#ifdef __EMSCRIPTEN__
std::function<void ()> loop;
void browser_loop();
void browser_loop() {
    loop();
}
#endif


int main() {
    ViewParams view_params;
    view_params.view_ratio = 512/view_params.width;
    GLFWwindow *window = init_window(view_params.view_ratio*view_params.width, 
                                     view_params.view_ratio*view_params.height);
    init_glew();
    glViewport(0, 0, view_params.width, view_params.height);

    ShaderPrograms shader_programs;
    shader_programs.init();

    MouseClick left_click = MouseClick(GLFW_MOUSE_BUTTON_1);

    Quad view_frame = Quad::make_frame(view_params.view_ratio*view_params.width, 
                                       view_params.view_ratio*view_params.height);
    Quad pot_frame = Quad::make_float_frame(view_params.width, view_params.height);
    Quad psi_frame = Quad::make_float_frame(view_params.width, view_params.height);

    SimParams sim_params;

    PotentialParams potential_params;
    potential_params.a = 20.0;
    potential_params.spacing = 0.4;
    int preset_potentials[4] = {
        0, PotentialParams::SHO, 
        PotentialParams::CIRCLE, PotentialParams::CONE
    };
    int n_preset_potentials = 4;

    int n_states = 50;
    int which_preset_potential = 2;
    int which_eigenvector = 0;
    VectorXcd eigenvalues;
    MatrixXcd eigenvectors;
    auto rgba_vector = std::vector<ComplexFloatRGBA>(
        view_params.width*view_params.height);

    auto select_preset_potential = [&](int potential_type) {
        size_t matrix_size = view_params.width*view_params.height;
        std::vector<Triplet<double>> triplets;
        SparseMatrix<double> mat(matrix_size, matrix_size);
        triplets.reserve(matrix_size*5);
        potential_params.potential_type = potential_type;
        MatrixXd potential = init_potential(potential_params,
                                            view_params.width,
                                            view_params.height);
        double_matrix_to_rgba_array((ComplexFloatRGBA *)&rgba_vector[0].r, 
                                    potential,
                                    view_params.width, view_params.height); 
        pot_frame.substitute_array(view_params.width, view_params.height,
                                   GL_FLOAT, (float *)&rgba_vector[0].r);
        init_energy_states(mat, triplets, eigenvalues, eigenvectors, n_states,
                           potential, sim_params, 
                           view_params.width, view_params.height);
        vector_slice_to_rgba_array((ComplexFloatRGBA *)&rgba_vector[0].r, 
                                   100.0, eigenvectors, which_eigenvector,
                                   view_params.width, view_params.height);
        psi_frame.substitute_array(view_params.width, view_params.height,
                                GL_FLOAT, (float *)&rgba_vector[0].r);
    };
    select_preset_potential(preset_potentials[which_preset_potential]);

    ViewFrameReads view_frame_reads;
    view_frame_reads.frame1 = &psi_frame;
    view_frame_reads.frame2 = &psi_frame;
    view_frame_reads.frame3 = &psi_frame;
    view_frame_reads.frame_potential = &pot_frame;

    int left_right[2];
    int up_down[2];


    #ifndef __EMSCRIPTEN__
    auto
    #endif
    loop = [&] {
        if (left_click.released) {

            ///

        }
        if (left_right[0] != 0 && left_right[1] == 0) {
            which_eigenvector = (which_eigenvector + left_right[0]) % n_states;
            if (which_eigenvector < 0) {
                which_eigenvector = n_states + which_eigenvector;
            }
            vector_slice_to_rgba_array((ComplexFloatRGBA *)&rgba_vector[0].r, 
                                       100.0, eigenvectors, which_eigenvector,
                                       view_params.width, view_params.height);
            psi_frame.substitute_array(view_params.width, view_params.height,
                                       GL_FLOAT, (float *)&rgba_vector[0].r);
        }
        if (up_down[0] != 0 && up_down[1] == 0) {
            which_preset_potential = (which_preset_potential
                                      + up_down[0]) % n_preset_potentials;
            if (which_preset_potential < 0) {
                which_preset_potential = n_preset_potentials
                                         + which_preset_potential;

            }
            select_preset_potential(preset_potentials[
                which_preset_potential]);
        }
        glViewport(0, 0, view_params.view_ratio*view_params.width, 
                   view_params.view_ratio*view_params.height); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_view(view_frame, view_frame_reads, shader_programs.view);

        glViewport(0, 0, view_params.width, view_params.height);

        ///

        glfwPollEvents();
        left_right[0] = 0;
        up_down[0] = 0;
        button_update(window, GLFW_KEY_LEFT, left_right[0], -1);
        button_update(window, GLFW_KEY_RIGHT, left_right[0], 1);
        button_update(window, GLFW_KEY_UP, up_down[0], 1);
        button_update(window, GLFW_KEY_DOWN, up_down[0], -1);
        std::swap(left_right[0], left_right[1]);
        std::swap(up_down[0], up_down[1]);
        left_click.update(window);
        glfwSwapBuffers(window);
    };

    #ifndef __EMSCRIPTEN__
    while (!glfwWindowShouldClose(window)) {
        loop();
    }
    #else
    emscripten_set_main_loop(browser_loop, 0, true);
    #endif
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}
