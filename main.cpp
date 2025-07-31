#include <Eigen/Core>
#include "gl_wrappers.hpp"
#include "glfw_window.hpp"
#include "simulation.hpp"
#include "interactor.hpp"
#include "parse.hpp"
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#endif
#include <functional>
#include <utility>

#include "wasm_wrappers.hpp"


static std::function <void()> s_loop;
#ifdef __EMSCRIPTEN__
static void s_main_loop() {
    s_loop();
}
#endif


void modify_reset_params(sim_2d::SimParams &dst, sim_2d::SimParams &mod) {
    dst.m = mod.m;
    dst.gridWidth = mod.gridWidth;
    dst.gridHeight = mod.gridHeight;
    dst.numberOfStates = mod.numberOfStates;
}


void display_parameters_as_sliders(int c, std::map<std::string, double> m) {
    std::string string_val = "[";
    m.erase("x");
    m.erase("y");
    for (auto &e: m)
        string_val += "\"" + e.first + "\", ";
    string_val += "]";
    string_val 
        = "modifyUserSliders(" + std::to_string(c) + ", " + string_val + ");";
    printf("%s\n", &string_val[0]);
    #ifdef __EMSCRIPTEN__
    emscripten_run_script(&string_val[0]);
    #endif
}

void ees_2d(
    MainGLFWQuad main_render,
    sim_2d::SimParams &params,
    int window_width, int window_height,
    unsigned int filter_type) {
    // test_shunting_yard();
    Interactor interactor(main_render.get_window());
    Simulation sim(window_width, window_height, filter_type, params);
    sim_2d::SimParams modified_params {};
    std::map<std::string, double> all_seen_variables {};
    Quaternion rotation = Quaternion{.i=0.0, .j=0.0, .k=0.0, .real=1.0};
    s_sim_params_set = [&params, &modified_params](int c, Uniform u) {
        if (c == params.GRID_WIDTH) {
            printf("Setting grid width and/or grid height: %d, %d.\n", params.gridWidth, params.gridHeight);
            modified_params.set(c, u);
        } else if (c == params.GRID_HEIGHT) {
            printf("Setting grid width and/or grid height: %d, %d.\n", params.gridWidth, params.gridHeight);
            modified_params.set(c, u);
        } else if (c == params.M) {
            modified_params.set(c, u);
        } else if (c == params.NUMBER_OF_STATES) {
            modified_params.set(c, u);
        } else {
            printf("Setting parameter with code %d.\n", c);
            printf("Parameter code is GRID_WIDTH: %d.\n", c == params.GRID_WIDTH);
            params.set(c, u);
        }
    };
    s_sim_params_get = [&params](int c) -> Uniform {
        return params.get(c);
    };
    s_sim_params_set_string = [&params](
        int c, int index, std::string val) {
        params.set(c, index, val);
    };
    s_button_pressed = [&params, &modified_params, 
        &sim, &all_seen_variables](int param_code) {
        if (param_code == params.ENTER_SCALAR_POTENTIAL) {
            std::string val = params.scalarPotential[0];
            std::map<std::string, double> variables_values 
                = sim.set_potential_from_string(params, val, 
                all_seen_variables);
            display_parameters_as_sliders(
                params.SCALAR_POTENTIAL, variables_values);
            // printf("Variable values\n");
            // for (auto& e: variables_values)
            //     printf("%s, %g\n", (char *)&e.first[0], e.second);
        }
        if (param_code == params.NORMALIZE_WAVE_FUNCTION) {
            sim.normalize_wave_function(params);
        }
        if (param_code == params.SIM_RESET_BUTTON) {
            modify_reset_params(params, modified_params);
            sim.freeze();
            sim.compute_new_energies(params);
        }
        // if (param_code == params.MEASURE_ENERGY) {
        //     sim.measure_energy(params);
        // }
    };
    s_sim_params_set_user_float_param = [&all_seen_variables](
        int c, std::string var_name, float value) {
        all_seen_variables.insert_or_assign(var_name, value);
    };
    s_user_edit_set_value
        = [](int c, std::string s, float value) {
        // 
    };
    s_user_edit_get_value
        = [](int c, std::string s) -> float {
        // auto uniforms = potential_edit.get_active_uniforms();
        // return uniforms.operator[](s).vec2[0];
    };
    s_selection_set
        = [&params, &sim, &all_seen_variables
        ](int c, int val) {
        if (c == params.PRESET_POTENTIAL) {
            params.presetPotential.selected = val;
            sim.set_preset_potential(
                params, 
                params.presetPotential.options[val]
            );
        }

    };
    s_image_set
        = [&params, &sim](
            int c, 
            const std::string &image_data, int width, int height) {
        sim.set_potential_from_image(params, (uint8_t *)&image_data[0],
        {.ind{width, height}});
    };
    std::vector<Vec2> start_position {};
    std::vector<Vec2> curr_position {};
    std::vector<std::pair<Vec2, Vec2>> start_double_touches {};
    std::vector<std::pair<Vec2, Vec2>> curr_double_touches {};
    s_loop = [&] {
        for (int i = 0; i < params.stepsPerFrame; i++)
            sim.time_step(params);
        if (start_position.size() > 0) {
            Vec2 cursor_pos1 = start_position[0];
            Vec2 cursor_pos2 = curr_position[curr_position.size() - 1];
            // if (cursor_pos1.x < 1.0 && cursor_pos2.x < 1.0) {
            //     cursor_pos1.x = cursor_pos1.x/(1.0/2.25);
            //     cursor_pos2.x = cursor_pos2.x/(1.0/2.25);
            // }
            if (cursor_pos1.x > 1.25/2.25) {
                cursor_pos1.x = (cursor_pos1.x - 1.25/2.25)/(1.0/2.25);
                cursor_pos2.x = (cursor_pos2.x - 1.25/2.25)/(1.0/2.25);
                sim.modify_stationary_state_coefficient(
                    params, cursor_pos1, cursor_pos2);
            } else if (cursor_pos2.x > 1.0/2.25 &&
                cursor_pos2.x <= 1.25/1.25) {
                cursor_pos1.x = (cursor_pos1.x - 1.0)/(0.25/2.25);
                cursor_pos2.x = (cursor_pos2.x - 1.0)/(0.25/2.25);
                sim.select_stationary_state_from_energy_levels(
                    params, cursor_pos2);
            } else if (cursor_pos1.x < 1.0 && cursor_pos2.x < 1.0) {
                cursor_pos1.x = cursor_pos1.x/(1.0/2.25);
                cursor_pos2.x = cursor_pos2.x/(1.0/2.25);
                if (params.show3D) {
                    if (curr_position.size() > 1) {
                        Vec2 delta_2d = interactor.get_mouse_delta();
                        Vec3 delta {.ind={delta_2d[0], delta_2d[1], 0.0}};
                        Vec3 view_vec {.ind={0.0, 0.0, -1.0}};
                        Vec3 axis = cross_product(delta, view_vec);
                        Quaternion rot = Quaternion::rotator(
                            3.0*axis.length(), axis);
                        rotation = rotation*rot;
                    }
                } else {
                    sim.approximate_wavepacket(
                        params, cursor_pos1, cursor_pos2);
                }
            }
        }
        // printf("Scroll value: %f\n", 0.5*Interactor::get_scroll());
        main_render.draw(sim.view(params, 
            rotation, 0.5*Interactor::get_scroll()));
        auto poll_events = [&] {
            glfwPollEvents();
            interactor.click_update(main_render.get_window());
            Vec2 pos = interactor.get_mouse_position();
            if (pos.x > 0.0 && pos.x < 1.0 && 
                pos.y > 0.0 && pos.y < 1.0 && interactor.left_pressed()) {
                if (start_position.empty())
                    start_position.push_back(pos);
                curr_position.push_back(pos);
            }
            if (interactor.left_released()) {
                if (!start_position.empty()) {
                    start_position.pop_back();
                    curr_position.clear();
                }
            }
            Vec2 double_touches[2];
            double_touches[0] = interactor.get_double_touch_position(0);
            double_touches[1] = interactor.get_double_touch_position(1);
            if (interactor.double_touch_active()
                && double_touches[0].x > 0.0 && double_touches[0].x < 1.0
                && double_touches[0].y > 0.0 && double_touches[0].y < 1.0
                && double_touches[1].x > 0.0 && double_touches[1].x < 1.0
                && double_touches[1].y > 0.0 && double_touches[1].y < 1.0) {
                // params.colorPhase = false;
                if (start_double_touches.empty())
                    start_double_touches.push_back(
                        {double_touches[0], double_touches[1]});
                curr_double_touches.push_back(
                        {double_touches[0], double_touches[1]});
                if (params.show3D && curr_double_touches.size() > 1) {
                    std::pair<Vec2, Vec2> delta01
                            = {
                                interactor.get_double_touch_delta(0),
                                interactor.get_double_touch_delta(1)
                            };
                    auto curr01 
                        = curr_double_touches[
                            curr_double_touches.size() - 1];
                    auto prev01 
                        = curr_double_touches[
                            curr_double_touches.size() - 2];
                    Vec2 dist_curr = curr01.second - curr01.first;
                    Vec2 dist_prev = prev01.second - prev01.first;
                    double curr_length = dist_curr.length();
                    double prev_length = dist_prev.length();
                    double scale_val = curr_length/prev_length;
                    Interactor::multiply_scroll_value(scale_val);
                    Vec2 mid = (curr01.second + curr01.first)/2.0;
                    Vec2 rad1 = (curr01.first - mid);
                    Vec2 rad2 = (curr01.second - mid);
                    Vec3 axis1 = cross_product(
                        Vec3{.x=delta01.first.x/rad1.length(),
                             .y=delta01.first.y/rad1.length(),
                             .z=0.0},
                        Vec3{.x=rad1.x, .y=rad1.y, .z=0.0}.normalized());
                    Vec3 axis2 = cross_product(
                        Vec3{.x=delta01.second.x/rad2.length(), 
                             .y=delta01.second.y/rad2.length(),
                             .z=0.0},
                        Vec3{.x=rad2.x, .y=rad2.y, .z=0.0}.normalized());
                    Quaternion rot = Quaternion::rotator(
                        0.33*(axis1 + axis2).length(), axis1 + axis2);
                    rotation = rotation*rot;
                }
            }
            if (interactor.double_touch_released()) {
                // params.colorPhase = true;
                if (!start_double_touches.empty()) {
                    start_double_touches.pop_back();
                    curr_double_touches.clear();
                }
            }
        };
        poll_events();
        glfwSwapBuffers(main_render.get_window());
    };
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(s_main_loop, 0, true);
    #else
    while (!glfwWindowShouldClose(main_render.get_window()))
        s_loop();
    #endif
}


int main(int argc, char *argv[]) {
    int window_width = 2304, window_height = 1024;
    int filter_type = GL_LINEAR;
    // Construct the main window quad
    if (argc >= 3) {
        window_width = std::atoi(argv[1]);
        window_height = std::atoi(argv[2]);
    }
    if (argc >= 4) {
        std::string s(argv[3]);
        if (s == "nearest")
            filter_type = GL_NEAREST;
    }
    auto main_quad = MainGLFWQuad(window_width, window_height);
    // UserEditGLSLProgram glsl_potential_edit {};
    sim_2d::SimParams sim_params {};
    ees_2d(
        main_quad, sim_params,
        window_width, window_height, filter_type);
    return 1;
}
