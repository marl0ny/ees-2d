#include "simulation.hpp"
#include "Eigen/src/Core/Matrix.h"
#include "circles_wireframe.hpp"
#include "clock_hands_wireframe.hpp"
#include "eigenstates2d.hpp"
#include "levels_wireframe.hpp"
#include "parse.hpp"
#include "surface.hpp"
#include <cstdint>
#include <random>



Programs::Programs() {
    this->domain_color = Quad::make_program_from_path(
        "./shaders/util/domain-color.frag"
    );
    this->uniform_color = Quad::make_program_from_path(
        "./shaders/util/uniform-color.frag"
    );
    this->circles = make_program_from_paths(
        "./shaders/gui/circles.vert", 
        "./shaders/gui/circles.frag"
    );
    this->clock_hands = make_program_from_paths(
        "./shaders/gui/clock-hands.vert",
        "./shaders/util/domain-color.frag"
    );
    this->levels = make_program_from_paths(
        "./shaders/gui/levels.vert",
        "./shaders/util/uniform-color.frag"
    );
    this->flip_horizontal = Quad::make_program_from_path(
        "./shaders/util/flip-horizontal.frag"
    );
    this->surface = make_program_from_paths(
        "./shaders/surface/surface.vert",
        "./shaders/surface/domain-coloring.frag"
    );
}

static IVec2 get_closest_larger_square(unsigned int n) {
    for (int i = 1; i; i++) {
        if (n > (i*i) && n <= ((i + 1)*(i + 1)))
            return IVec2{.ind{(i + 1), (i + 1)}}; 
    }
    return {.x=0, .y=0};
}

Frames::Frames(
    int window_width, int window_height,
    int sim_width, int sim_height, int n_states) :
    main_view_tex_params({
        .format=GL_RGBA16F, 
        .width=(uint32_t)window_width,
        .height=(uint32_t)window_height,
        .wrap_s=GL_CLAMP_TO_EDGE,
        .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=GL_LINEAR,
        .mag_filter=GL_LINEAR,
    }),
    sim_tex_params({
        .format=GL_RG32F, 
        .width=(uint32_t)sim_width,
        .height=(uint32_t)sim_height,
        .wrap_s=GL_CLAMP_TO_EDGE,
        .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=GL_LINEAR,
        .mag_filter=GL_LINEAR,
    }),
    main_render(main_view_tex_params),
    wave_func(sim_tex_params),
    coefficients(
        {
            .format=GL_RG32F, 
            // .width=(uint32_t)rectangular_root(n_states)[0],
            // .height=(uint32_t)rectangular_root(n_states)[1],
            .width=(uint32_t)get_closest_larger_square(n_states)[0],
            .height=(uint32_t)get_closest_larger_square(n_states)[1],
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_LINEAR,
            .mag_filter=GL_LINEAR,
        }
    ),
    coefficients_tmp(
        {
            .format=GL_RG32F, 
            // .width=(uint32_t)rectangular_root(n_states)[0],
            // .height=(uint32_t)rectangular_root(n_states)[1],
            .width=(uint32_t)get_closest_larger_square(n_states)[0],
            .height=(uint32_t)get_closest_larger_square(n_states)[1],
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_LINEAR,
            .mag_filter=GL_LINEAR,
        }
    ),
    energies({
        {
            .format=GL_RG32F,
            .width=(uint32_t)n_states, .height=1,
            .wrap_s=GL_REPEAT,
            .wrap_t=GL_REPEAT,
            .min_filter=GL_LINEAR,
            .mag_filter=GL_LINEAR
        }
    }),
    circles(get_circles_wireframe(
        get_closest_larger_square(n_states)
        // rectangular_root(n_states)
    )),
    clock_hands(get_clock_hands_wireframe(
        get_closest_larger_square(n_states)
        // rectangular_root(n_states)
    )),
    levels(
        get_levels_wireframe(n_states)
    ),
    surface(
        get_surface_wireframe({.ind{512, 512}})
    )
    {}

void Frames::reset(int n_states) {
    this->coefficients.reset(
        {
            .format=GL_RG32F, 
            // .width=(uint32_t)rectangular_root(n_states)[0],
            // .height=(uint32_t)rectangular_root(n_states)[1],
            .width=(uint32_t)get_closest_larger_square(n_states)[0],
            .height=(uint32_t)get_closest_larger_square(n_states)[1],
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_LINEAR,
            .mag_filter=GL_LINEAR,
        }
    );
    this->coefficients_tmp.reset(
        {
            .format=GL_RG32F, 
            // .width=(uint32_t)rectangular_root(n_states)[0],
            // .height=(uint32_t)rectangular_root(n_states)[1],
            .width=(uint32_t)get_closest_larger_square(n_states)[0],
            .height=(uint32_t)get_closest_larger_square(n_states)[1],
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_LINEAR,
            .mag_filter=GL_LINEAR,
        }
    ),
    this->energies.reset(
        {
            .format=GL_RG32F,
            .width=(uint32_t)n_states, .height=1,
            .wrap_s=GL_REPEAT,
            .wrap_t=GL_REPEAT,
            .min_filter=GL_LINEAR,
            .mag_filter=GL_LINEAR
        }
    );
    this->circles = get_circles_wireframe(
        get_closest_larger_square(n_states)
        // rectangular_root(n_states)
    );
    this->clock_hands = get_clock_hands_wireframe(
        get_closest_larger_square(n_states)
        // rectangular_root(n_states)
    );
    this->levels = get_levels_wireframe(n_states);
}

void Simulation::compute_new_energies(sim_2d::SimParams &params) {
    if (params.numberOfStates != m_coefficients.size()) {
        // printf("Number of states: %d, %d\n", 
        //        params.numberOfStates, m_coefficients.size());
        m_frames.reset(params.numberOfStates);
        // m_initial_coefficients = Eigen::Vector
        m_initial_coefficients.resize(params.numberOfStates);
        m_coefficients.resize(params.numberOfStates);
        m_energy_eigenvalues.resize(params.numberOfStates);
        m_energy_eigenstates.resize(
            params.gridWidth*params.gridHeight,
            params.numberOfStates
        );
        for (int i = 0; i < params.numberOfStates; i++) {
            m_initial_coefficients[i] = 0.0;
            m_coefficients[i] = 0.0;
        }
        m_initial_coefficients[0] = 1.0;
    }
    init_energy_states(
        m_energy_eigenvalues, m_energy_eigenstates,
        m_potential,
        {
            .hbar=params.hbar, .m=params.m,
            .width=params.width, .height=params.height,
            .grid_width=params.gridWidth, .grid_height=params.gridHeight,
            .n_states=params.numberOfStates,
            .laplacian_stencil=params.laplacianStencil.selected
        }
    );
    std::vector<std::complex<float>> energies {};
    for (int i = 0; i < params.numberOfStates; i++)
        energies.push_back(
            (std::complex<float>)m_energy_eigenvalues[i]
        );
    m_frames.energies.set_pixels((float *)&energies[0]);
    this->is_recomputing = false;
}

void Simulation::config_at_start(sim_2d::SimParams &params) {
    m_initial_coefficients = Eigen::VectorXcd(params.numberOfStates);
    m_coefficients = Eigen::VectorXcd(params.numberOfStates);
    m_wave_function = Eigen::VectorXcf(params.gridWidth*params.gridHeight);
    for (int i = 0; i < params.numberOfStates; i++) {
        m_initial_coefficients[i] = 0.0;
        if (i == 0)
            m_initial_coefficients[i] 
                = (1.0 + std::complex<double>(0.0, 1.0))/sqrt(2.0);
        if (i == 1)
            m_initial_coefficients[i]
                 = (1.0 + std::complex<double>(0.0, -1.0))/sqrt(2.0);
    }
    // std::complex<double> norm_factor
    //      = m_coefficients.adjoint()*m_coefficients;
    // for (int i = 0; i < params.numberOfStates; i++) {

    // }
    this->m_potential = Eigen::MatrixXd(params.gridWidth, params.gridHeight);
    this->set_potential_from_string(
        params, "2.5*(x^2 + y^2)", {});
}

void Simulation::modify_stationary_state_coefficient(
    sim_2d::SimParams &params, Vec2 cursor_pos1, Vec2 cursor_pos2) {
    // IVec2 widgets_dim = rectangular_root(params.numberOfStates);
    IVec2 widgets_dim = get_closest_larger_square(params.numberOfStates);
    float dx = 1.0/widgets_dim[0];
    float dy = 1.0/widgets_dim[1];
    int x_offset = int(cursor_pos1.x/dx);
    int y_offset = int((1.0 - cursor_pos1.y)/dy);
    int location = y_offset*widgets_dim[0] + x_offset;
    float subsquare_x 
        = cursor_pos2.x*widgets_dim[0] - int(cursor_pos1.x/dx) - 0.5;
    float subsquare_y 
        = cursor_pos2.y*widgets_dim[1] - int(cursor_pos1.y/dy) - 0.5;
    Vec2 subsquare_coord = Vec2{.x=subsquare_x, .y=subsquare_y};
    std::vector<std::complex<double>> coefficients(params.numberOfStates, 0);
    for (int i = 0; i < params.numberOfStates; i++) {
        coefficients[i] = m_coefficients[params.numberOfStates - i - 1];
        if (i == location) {
            if (subsquare_coord.length_squared() > 0.4*0.4)
                subsquare_coord = 0.4*subsquare_coord.normalized();
            if (subsquare_coord.length_squared() < 0.04*0.04)
                coefficients[i] = 0.0;
            else
                coefficients[i] = std::complex<double>(
                    (subsquare_coord/0.4).x, (subsquare_coord/0.4).y);
        }
        m_initial_coefficients[params.numberOfStates - i - 1]
             = coefficients[i];
    }
    params.t = 0.0;
    // printf("%f, %f\n", x, y);
}

void Simulation::select_stationary_state_from_energy_levels(
    sim_2d::SimParams &params, Vec2 cursor_pos
) {
    double max_level = m_energy_eigenvalues[0].real();
    double min_level
         = m_energy_eigenvalues[m_energy_eigenvalues.size() - 1].real();
    double level
        = ((cursor_pos.y - 0.05)/0.9)*(max_level - min_level) + min_level;
    // double sigma = 0.01*(max_level - min_level);
    std::complex<double> diff = level - m_energy_eigenvalues[0];
    int closest_level_index = 0;
    for (int i = 1; i < params.numberOfStates; i++) {
        std::complex<double> diff2 = level - m_energy_eigenvalues[i];
        if (abs(diff2*conj(diff2)) < abs(diff*conj(diff))) {
            diff = diff2;
            closest_level_index = i;
        }
    }
    if (sqrt(abs(diff*conj(diff))) <= 0.01*(max_level - min_level)) {
        std::vector<std::complex<double>> initial_coefficients {}; 
        double norm_val = 0.0;
        std::complex<double> closest_level 
            = m_energy_eigenvalues[closest_level_index];
        std::complex<double> val = 1.0;
        for (int i = 0; i < params.numberOfStates; i++) {
            std::complex<double> diff2 
                = closest_level - m_energy_eigenvalues[i];
            if (sqrt(abs(diff2*conj(diff2))) 
                < 0.001*(max_level - min_level)) {
                initial_coefficients.push_back(val);
                // val *= std::complex<double>(0.0, 1.0);
            } else {
                initial_coefficients.push_back(0.0);
            }
            // initial_coefficients.push_back(
            //     std::exp(-0.5*pow((level
            //  - m_energy_eigenvalues[i])/sigma, 2.0)));
            norm_val += pow(initial_coefficients[i].real(), 2.0);
        }
        if (norm_val > 0.5) {
            params.t = 0.0;
            for (int i = 0; i < params.numberOfStates; i++)
                m_initial_coefficients[i] 
                    = initial_coefficients[i]/sqrt(norm_val);
        }
    }
    printf("y position: %g\n", cursor_pos.y);
    printf("Selected level: %g\n", level);
}

void Simulation::approximate_wavepacket(
    sim_2d::SimParams &params,
    Vec2 cursor_pos1, Vec2 cursor_pos2
) {
    double pi = 3.141592653589793;
    std::complex<double> imag_unit {0.0, 1.0};
    Vec2 diff = cursor_pos2 - cursor_pos1;
    double nx = diff.x*params.gridWidth/8.0;
    double ny = diff.y*params.gridHeight/8.0;
    Eigen::VectorXcd wave_packet (params.gridWidth*params.gridHeight);
    double sigma = 0.1;
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = (double)j/(double)params.gridWidth - cursor_pos1.x;
            double y = (double)i/(double)params.gridHeight - cursor_pos1.y;
            wave_packet[i*params.gridWidth + j]
                = exp(-0.5*(x*x + y*y)/(sigma*sigma))
                * exp(imag_unit*(pi*x*nx + pi*y*ny));
        }
    }
    std::complex<double> norm_val = 0.0;
    double largest_val = 0.0;
    for (int n = 0; n < params.numberOfStates; n++) {
        std::complex<double> coeff = 0;
        for (int i = 0; i < params.gridHeight; i++) {
            for (int j = 0; j < params.gridWidth; j++) {
                coeff += wave_packet[i*params.gridWidth + j]
                    * conj(m_energy_eigenstates(i*params.gridWidth + j, n));
            }
        }
        largest_val = (abs(coeff*conj(coeff)) > largest_val)? 
            abs(coeff*conj(coeff)): largest_val;
        norm_val += coeff*conj(coeff);
        m_initial_coefficients[n] = coeff;
        params.t = 0.0;
    }
    for (int n = 0; n < params.numberOfStates; n++) {
        m_initial_coefficients[n] /= sqrt(largest_val);
        // m_initial_coefficients[n] /= sqrt(norm_val);
    }
}

std::map<std::string, double> Simulation::set_potential_from_string(
    sim_2d::SimParams &params, const std::string &string_val,
    std::map<std::string, double> user_params) {
    std::vector<std::string> expr_stack = get_expression_stack(string_val);
    std::vector<std::string> rpn_list = shunting_yard(expr_stack);
    // std::cout << "rpn list: \n";
    // for (auto &e: rpn_list)
    //     std::cout << e << std::endl;
    // std::cout << std::endl;
    std::set<std::string> variables 
        = get_variables_from_rpn_list(rpn_list);
    // std::cout << "variables: \n";
    std::map<std::string, double> variables_values;
    for (auto &variable: variables)
        variables_values.insert({variable, 1.0F});
    for (auto &e: user_params)
        if (variables_values.count(e.first))
            variables_values.insert_or_assign(e.first, e.second);
    std::map<std::string, double> ret_val {};
    for (auto &e: variables_values)
        ret_val.insert(e);
    // variables_values.insert({"x", 0.0});
    // variables_values.insert({"y", 0.0});
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = params.width 
                * (((double)j + 0.5)/(double)params.gridWidth - 0.5);
            double y = params.height
                 * (((double)i + 0.5)/(double)params.gridHeight - 0.5);
            variables_values.insert_or_assign("x", x);
            variables_values.insert_or_assign("y", y);
            double val = compute_expression(rpn_list, variables_values);
            // if (i == params.gridWidth - 1
            //     && j == params.gridHeight - 1) {
            //     std::cout << "Expression: " << string_val << std::endl;
            //     std::cout << "Variables: " << std::endl;
            //     for (auto &e: variables_values) {
            //         std::cout << e.first << ": " << e.second << std::endl;
            //     }
            //     std::cout << "Value: " << val << std::endl;
            // }
            m_potential(i, j) = val;
        }
    }
    this->compute_new_energies(params);
    return ret_val;
}

Simulation::Simulation(int window_width, int window_height, sim_2d::SimParams params):
    m_programs(),
    m_frames(
        window_width, window_height, 
        params.gridWidth, params.gridHeight, params.numberOfStates) {
    this->config_at_start(params);
}

void Simulation::freeze() {
    this->is_recomputing = true;
}

void Simulation::time_step(sim_2d::SimParams &params) {
    if (this->is_recomputing)
        return;
    double hbar = params.hbar;
    double t = params.t;
    for (int k = 0; k < params.numberOfStates; k++) {
        std::complex<double> energy = m_energy_eigenvalues[k];
        std::complex<double> i (std::complex<double> (0.0, 1.0));
        m_coefficients(k) 
            = m_initial_coefficients(k)*std::exp(-i*energy*t/hbar);
    }
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            m_wave_function(i*params.gridWidth + j) = 0.0;
            for (int k = 0; k < params.numberOfStates; k++) {
                m_wave_function(i*params.gridWidth + j) 
                    += m_coefficients(k)
                        * m_energy_eigenstates(i*params.gridWidth + j, k);
            }
            m_wave_function(i*params.gridWidth + j) 
                *= sqrt(params.gridWidth*params.gridHeight);
        }
    }
    int size = m_frames.coefficients.height()*m_frames.coefficients.width();
    auto coefficients_f = Eigen::VectorXcf(size);
    for (int k = size - 1; k >= 0; k--) {
        int j = k - (size - params.numberOfStates);
        coefficients_f[k] = (j >= 0)? m_coefficients[j]: 0.0;
    }
    m_frames.coefficients_tmp.set_pixels((float *)&coefficients_f[0]);
    m_frames.coefficients.draw(
        m_programs.flip_horizontal,
        {{"tex", &m_frames.coefficients_tmp}}
    );
    params.t += (params.invertTimeStep? -1.0: 1.0)*params.dt;
}

void Simulation::normalize_wave_function(const sim_2d::SimParams &params) {
    std::complex<double> norm_val = 0.0;
    for (int i = 0; i < params.numberOfStates; i++)
        norm_val += m_coefficients[i]*conj(m_coefficients[i]);
    for (int i = 0; i < params.numberOfStates; i++)
        m_initial_coefficients[i] /= sqrt(norm_val.real());
}

double Simulation::measure_energy(const sim_2d::SimParams &params) {
    // Eigen::MatrixXd coefficients_squared(params.numberOfStates);
    double norm_val = 0.0;
    for (int i = 0; i < params.numberOfStates; i++) {
         norm_val += std::real(m_coefficients[i]*conj(m_coefficients[i]));
    }
    std::random_device rand_device;
    std::default_random_engine rand_engine (rand_device());
    std::uniform_real_distribution<double> rand(0.0, 1.0);
    double rand_val = rand(rand_engine);
    double val = 0.0;
    double energy = 0.0;
    int index = 0;
    for (int i = 0; i < params.numberOfStates; i++) {
        val += std::abs(m_coefficients[i])*std::abs(m_coefficients[i])
            / norm_val;
        if (val >= rand_val) {
            energy = std::abs(m_energy_eigenvalues[i]);
            index = i;
            break;
        }
    }
    for (int i = 0; i < params.numberOfStates; i++)
        m_initial_coefficients[i] = (i == index)? 1.0: 0.0;
    return energy;
}

static WireFrame get_quad_wire_frame() {
    return WireFrame(
        {{"position", Attribute{
            3, GL_FLOAT, false,
            0, 0}}},
        {-1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -1.0, 0.0},
        {0, 1, 2, 0, 2, 3},
        WireFrame::TRIANGLES
    );
}

static double compute_energy_expectation_value(
    const Eigen::VectorXcd &coefficients,
    const Eigen::VectorXcd &energy_eigenvalues,
    int number_of_levels
) {
    std::complex<double> exp = 0.0;
    std::complex<double> norm_val = 0.0;
    for (int i = 0; i < number_of_levels; i++) {
        exp += coefficients[i]*std::conj(coefficients[i])
            *energy_eigenvalues[i];
        norm_val += coefficients[i]*std::conj(coefficients[i]);
    }
    return exp.real()/norm_val.real();
}

static double get_level_pos(
    double level, double min_level, double max_level) {
    return (level - min_level)/(max_level - min_level);
}

const RenderTarget& Simulation::view(
    sim_2d::SimParams &params, ::Quaternion rotation, float scale) {
    m_frames.wave_func.set_pixels((float *)&m_wave_function[0]);
    WireFrame quad_wire_frame = get_quad_wire_frame();
    m_frames.main_render.clear();
    if (params.show3D) {
        IVec2 screen_dimensions = {.ind{
            (int)m_frames.main_view_tex_params.height,
            (int)m_frames.main_view_tex_params.height
        }};
        glEnable(GL_DEPTH_TEST);
        m_frames.main_render.draw(
            m_programs.surface,
            {
                {"heightTex", &m_frames.wave_func},
                {"rotation", rotation},
                {"screenDimensions", screen_dimensions},
                {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
                {"heightScale", 0.001F},
                {"scale", scale},
                {"dimensions2D", 
                    IVec2{.ind{512, 512}}},
                {"tex", &m_frames.wave_func},
                {"brightness", params.brightness},
                {"heightDataType", int(1)}
            },
            m_frames.surface,
            Config::viewport(
                0, 0, 
                m_frames.main_view_tex_params.height, 
                m_frames.main_view_tex_params.height)
        );
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        m_frames.main_render.draw(
            m_programs.domain_color,
            {
                {"tex", &m_frames.wave_func},
                {"brightness", float(params.brightness)},
                {"encodeMagAsBrightness", 1},
                {"index", 0}
            },
            quad_wire_frame,
            Config::viewport(
                0, 0, 
                m_frames.main_view_tex_params.height, 
                m_frames.main_view_tex_params.height)
        );
    }
    float min_level = float(m_energy_eigenvalues[
        params.numberOfStates - 1].real());
    float max_level = float(m_energy_eigenvalues[0].real());
    double expectation_value
        = compute_energy_expectation_value(
            m_coefficients, m_energy_eigenvalues,
            params.numberOfStates);
    // printf("Min/Max energy levels: %g, %g\n", min_level, max_level);
    // printf("Energy expectation value %g\n", expectation_value);
    m_frames.main_render.draw(
        m_programs.levels,
        {
            {"vertTex", &m_frames.energies},
            {"expectationValue", float(expectation_value)},
            {"minLevel", (float)min_level},
            {"maxLevel", (float)max_level},
            {"color", Vec4{.r=0.8, .g=0.8, .b=0.8, .a=0.8}}
        },
        m_frames.levels,
        Config::viewport(
            m_frames.main_view_tex_params.height, 0, 
            m_frames.main_view_tex_params.height/4, 
            m_frames.main_view_tex_params.height)
    );
    {
        WireFrame tmp = get_quad_wire_frame();
        double level_pos 
            = get_level_pos(expectation_value, min_level, max_level);
        m_frames.main_render.draw(
            m_programs.uniform_color,
            {
                {"color", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=1.0}},
            },
            tmp,
            Config::viewport(
                int(m_frames.main_view_tex_params.height*(1.0 + 0.05*0.25)),
                int(m_frames.main_view_tex_params.height*
                       (0.9*level_pos + 0.05)) - 1,
                int(0.9*(m_frames.main_view_tex_params.height/4.0)),
                3
            )
        );
    }
    /*
    uniform ivec2 circlesGridSize;
    uniform int inUseCount;
    */
    m_frames.main_render.draw(
        m_programs.circles,
        {
            {"color", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=1.0}},
            {"circlesGridSize", IVec2{.ind{
            (int)m_frames.coefficients.width(),
            (int)m_frames.coefficients.height()}}},
            {"inUseCount", int(params.numberOfStates)}
        },
        m_frames.circles,
        Config::viewport(
            m_frames.main_view_tex_params.height*1.25, 0, 
            m_frames.main_view_tex_params.height, 
            m_frames.main_view_tex_params.height)
    );
    m_frames.main_render.draw(
        m_programs.clock_hands,
        {
            {"vertTex", &m_frames.coefficients},
            {"tex", &m_frames.coefficients},
            {"brightness", 1.0F},
            {"index", 0}
        },
        m_frames.clock_hands,
        Config::viewport(
            m_frames.main_view_tex_params.height*1.25, 0, 
            m_frames.main_view_tex_params.height, 
            m_frames.main_view_tex_params.height)
    );
    return m_frames.main_render;
}