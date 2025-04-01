#include "gl_wrappers2d/gl_wrappers.hpp"
#include <GLFW/glfw3.h>
#include "shaders.hpp"


#pragma once

struct ShaderPrograms {

    GLuint cn_explicit_part = 0;
    GLuint complex_multiply = 0;
    GLuint copy_scale = 0;
    GLuint exp_potential = 0;
    GLuint initial_wavepacket = 0;
    GLuint initial_potential = 0;
    GLuint jacobi_iteration = 0;
    GLuint prob_current = 0;
    GLuint real_imag_timestep = 0;
    GLuint view = 0;

    void init() {


         #ifndef __EMSCRIPTEN__
        GLuint vertices_shader = get_shader("./shaders/vertex.vert", GL_VERTEX_SHADER);
        GLuint cn_explicit_part_shader = get_shader("./shaders/cn-explicit-part.frag", GL_FRAGMENT_SHADER);
        GLuint complex_multiply_shader = get_shader("./shaders/complex-multiply.frag", GL_FRAGMENT_SHADER);
        GLuint copy_scale_shader = get_shader("./shaders/copy-scale.frag", GL_FRAGMENT_SHADER);
        GLuint exp_potential_shader = get_shader("./shaders/exp-potential.frag", GL_FRAGMENT_SHADER);
        GLuint initial_wavepacket_shader = get_shader("./shaders/initial-wavepacket.frag", GL_FRAGMENT_SHADER);
        GLuint jacobi_iteration_shader = get_shader("./shaders/jacobi-iteration.frag", GL_FRAGMENT_SHADER);
        GLuint prob_current_shader = get_shader("./shaders/prob-current.frag", GL_FRAGMENT_SHADER);
        GLuint real_imag_timestep_shader = get_shader("./shaders/real-imag-timestep.frag", GL_FRAGMENT_SHADER);
        GLuint view_frame_shader = get_shader("./shaders/view-frame.frag", GL_FRAGMENT_SHADER);
        GLuint initial_potential_shader = get_shader("./shaders/initial-potential.frag", GL_FRAGMENT_SHADER);
        #else
        GLuint vertices_shader = make_vertex_shader(vertex_shader_source);
        GLuint cn_explicit_part_shader = make_fragment_shader(cn_explicit_part_shader_source);
        GLuint complex_multiply_shader = make_fragment_shader(complex_multiply_shader_source);
        GLuint copy_scale_shader = make_fragment_shader(copy_scale_shader_source);
        GLuint exp_potential_shader = make_fragment_shader(exp_potential_shader_source);
        GLuint initial_wavepacket_shader = make_fragment_shader(initial_wavepacket_shader_source);
        GLuint jacobi_iteration_shader = make_fragment_shader(jacobi_iteration_shader_source);
        GLuint prob_current_shader = make_fragment_shader(prob_current_shader_source);
        GLuint real_imag_timestep_shader = make_fragment_shader(real_imag_timestep_shader_source);
        GLuint view_frame_shader = make_fragment_shader(view_frame_shader_source);
        GLuint initial_potential_shader = make_fragment_shader(initial_potential_shader_source);
        #endif

        cn_explicit_part = make_program(vertices_shader, cn_explicit_part_shader);
        complex_multiply = make_program(vertices_shader, complex_multiply_shader);
        copy_scale = make_program(vertices_shader, copy_scale_shader);
        exp_potential = make_program(vertices_shader, exp_potential_shader);
        initial_wavepacket = make_program(vertices_shader, initial_wavepacket_shader);
        jacobi_iteration = make_program(vertices_shader, jacobi_iteration_shader);
        prob_current = make_program(vertices_shader, prob_current_shader);
        real_imag_timestep = make_program(vertices_shader, real_imag_timestep_shader);
        initial_potential = make_program(vertices_shader, initial_potential_shader);
        view = make_program(vertices_shader, view_frame_shader);

        // for e in $(egrep shader_programs.hpp -oh -e "[a-zA-Z_]+_shader[ ]"); do echo "GLuint $e;"; done
        auto shaders = {
            vertices_shader,
            cn_explicit_part_shader,
            complex_multiply_shader,
            copy_scale_shader,
            exp_potential_shader,
            initial_wavepacket_shader,
            jacobi_iteration_shader,
            prob_current_shader,
            real_imag_timestep_shader,
            view_frame_shader,
            vertices_shader,
            cn_explicit_part_shader,
            complex_multiply_shader,
            copy_scale_shader,
            exp_potential_shader,
            initial_wavepacket_shader,
            jacobi_iteration_shader,
            prob_current_shader,
            real_imag_timestep_shader,
            initial_potential_shader,
            view_frame_shader,
        };

        for (auto s: shaders) {
            glDeleteShader(s);
        }
    }
};