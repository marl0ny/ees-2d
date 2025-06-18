
#include "parameters.hpp"

#ifndef _IMGUI_CONTROLS_
#define _IMGUI_CONTROLS_
using namespace sim_2d;

#include "gl_wrappers.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <functional>
#include <set>

#include "parameters.hpp"

static std::function<void(int, Uniform)> s_sim_params_set;
static std::function<void(int, int, std::string)> s_sim_params_set_string;
static std::function<Uniform(int)> s_sim_params_get;
static std::function<void(int, std::string, float)> s_user_edit_set_value;
static std::function<float(int, std::string)> s_user_edit_get_value;
static std::function<std::string(int)>
    s_user_edit_get_comma_separated_variables;
static std::function<void(int)> s_button_pressed;
static std::function<void(int, int)> s_selection_set;
static std::function<void(int, std::string, float)>
    s_sim_params_set_user_float_param;

static ImGuiIO global_io;
static std::map<int, std::string> global_labels;

void edit_label_display(int c, std::string text_content) {
    global_labels[c] = text_content;
}

void display_parameters_as_sliders(
    int c, std::set<std::string> variables) {
    std::string string_val = "[";
    for (auto &e: variables)
        string_val += """ + e + "", ";
    string_val += "]";
    string_val 
        = "modifyUserSliders(" + std::to_string(c) + ", " + string_val + ");";
    // TODO
}

void start_gui(void *window) {
    bool show_controls_window = true;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void imgui_controls(void *void_params) {
    SimParams *params = (SimParams *)void_params;
    for (auto &e: global_labels)
        params->set(e.first, 0, e.second);
    if (ImGui::SliderFloat("Brightness", &params->brightness, 0.0, 2.0))
           s_sim_params_set(params->BRIGHTNESS, params->brightness);
    if (ImGui::SliderFloat("Time step (a.u.)", &params->dt, 0.0, 0.1))
           s_sim_params_set(params->DT, params->dt);
    ImGui::Checkbox("Negative time step", &params->invertTimeStep);
    ImGui::Checkbox("Colour phase", &params->colorPhase);
    ImGui::Checkbox("3D plot", &params->show3D);
    ImGui::Text("Plot heights");
    if (ImGui::SliderFloat("heightScales[0]", &params->heightScales.ind[0], 0.0, 10.0))
           s_sim_params_set(params->HEIGHT_SCALES, params->heightScales);
    if (ImGui::SliderFloat("heightScales[1]", &params->heightScales.ind[1], 0.0, 10.0))
           s_sim_params_set(params->HEIGHT_SCALES, params->heightScales);
    if (ImGui::SliderFloat("V(x, y) brightness", &params->brightness2, 0.0, 0.5))
           s_sim_params_set(params->BRIGHTNESS2, params->brightness2);
    ImGui::Text("--------------------------------------------------------------------------------");
    ImGui::Text("Reset parameters");
    ImGui::Text("(Press the 'Reset simulation' button for changes to take effect.)");
    if (ImGui::SliderInt("Energy eigenstates count", &params->numberOfStates, 1, 256))
            s_sim_params_set(params->NUMBER_OF_STATES, params->numberOfStates);
    if (ImGui::BeginMenu("Laplacian discretization")) {
        if (ImGui::MenuItem( "2nd order 5 pt."))
            s_selection_set(params->LAPLACIAN_STENCIL, 0);
        if (ImGui::MenuItem( "4th order 9 pt."))
            s_selection_set(params->LAPLACIAN_STENCIL, 1);
        ImGui::EndMenu();
    }
    if (ImGui::SliderFloat("Mass (a.u.)", &params->m, 0.2, 10.0))
           s_sim_params_set(params->M, params->m);
    ImGui::Text("--------------------------------------------------------------------------------");
    if (ImGui::BeginMenu("Preset V(x, y)")) {
        if (ImGui::MenuItem( "x^2 + y^2"))
            s_selection_set(params->PRESET_POTENTIAL, 0);
        if (ImGui::MenuItem( "5*(x^2 + y^2)/2"))
            s_selection_set(params->PRESET_POTENTIAL, 1);
        if (ImGui::MenuItem( "sqrt(x^2 + y^2)"))
            s_selection_set(params->PRESET_POTENTIAL, 2);
        if (ImGui::MenuItem( "abs(x)"))
            s_selection_set(params->PRESET_POTENTIAL, 3);
        if (ImGui::MenuItem( "x^4 + y^4"))
            s_selection_set(params->PRESET_POTENTIAL, 4);
        if (ImGui::MenuItem( "(x+3/2)^2*(x-3/2)^2"))
            s_selection_set(params->PRESET_POTENTIAL, 5);
        if (ImGui::MenuItem( "5-5*exp(-(x^2+y^2)/9)"))
            s_selection_set(params->PRESET_POTENTIAL, 6);
        if (ImGui::MenuItem( "9-9*exp(-(x^2+y^2)/4)"))
            s_selection_set(params->PRESET_POTENTIAL, 7);
        if (ImGui::MenuItem( "-10*exp(-(x^2+y^2)/4)"))
            s_selection_set(params->PRESET_POTENTIAL, 8);
        if (ImGui::MenuItem( "Finite circular well"))
            s_selection_set(params->PRESET_POTENTIAL, 9);
        if (ImGui::MenuItem( "Heart"))
            s_selection_set(params->PRESET_POTENTIAL, 10);
        if (ImGui::MenuItem( "Triangle"))
            s_selection_set(params->PRESET_POTENTIAL, 11);
        if (ImGui::MenuItem( "Four overlapping wells"))
            s_selection_set(params->PRESET_POTENTIAL, 12);
        if (ImGui::MenuItem( "log(sqrt(x^2+y^2))"))
            s_selection_set(params->PRESET_POTENTIAL, 13);
        if (ImGui::MenuItem( "-log(sqrt(x^2+y^2))"))
            s_selection_set(params->PRESET_POTENTIAL, 14);
        if (ImGui::MenuItem( "-1/(sqrt(x^2+y^2))"))
            s_selection_set(params->PRESET_POTENTIAL, 15);
        if (ImGui::MenuItem( "1/(sqrt(x^2+y^2))"))
            s_selection_set(params->PRESET_POTENTIAL, 16);
        ImGui::EndMenu();
    }
    ImGui::Text("-5 a.u. ≤ x < 5 a.u.");
    ImGui::Text("-5 a.u. ≤ y < 5 a.u.");

}

bool outside_gui() {{
    return !global_io.WantCaptureMouse;
}}

void display_gui(void *data) {{
    global_io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    bool val = true;
    ImGui::Begin("Controls", &val);
    ImGui::Text("WIP AND INCOMPLETE");
    imgui_controls(data);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}}

#endif
