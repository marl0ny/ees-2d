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
    this->mag_color_map = Quad::make_program_from_path(
        "./shaders/util/mag-color-map.frag"
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
    this->surface_domain_color = make_program_from_paths(
        "./shaders/surface/surface.vert",
        "./shaders/surface/domain-coloring.frag"
    );
    this->surface_mag_color_map = make_program_from_paths(
        "./shaders/surface/surface.vert",
        "./shaders/surface/mag-color-map.frag"
    );
    this->surface_single_color = make_program_from_paths(
        "./shaders/surface/surface.vert",
        "./shaders/surface/single-color.frag"
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
    int sim_width, int sim_height, unsigned int filter_type, int n_states) :
    main_view_tex_params({
        .format=GL_RGBA16F, 
        .width=(uint32_t)window_width,
        .height=(uint32_t)window_height,
        .wrap_s=GL_CLAMP_TO_EDGE,
        .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=filter_type,
        .mag_filter=filter_type,
    }),
    sim_tex_params({
        .format=GL_RG32F, 
        .width=(uint32_t)sim_width,
        .height=(uint32_t)sim_height,
        .wrap_s=GL_CLAMP_TO_EDGE,
        .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=filter_type,
        .mag_filter=filter_type,
    }),
    main_render(main_view_tex_params),
    wave_func(sim_tex_params),
    potential(sim_tex_params),
    coefficients(
        {
            .format=GL_RG32F, 
            // .width=(uint32_t)rectangular_root(n_states)[0],
            // .height=(uint32_t)rectangular_root(n_states)[1],
            .width=(uint32_t)get_closest_larger_square(n_states)[0],
            .height=(uint32_t)get_closest_larger_square(n_states)[1],
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=filter_type,
            .mag_filter=filter_type,
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
            .min_filter=filter_type,
            .mag_filter=filter_type,
        }
    ),
    energies({
        {
            .format=GL_RG32F,
            .width=(uint32_t)n_states, .height=1,
            .wrap_s=GL_REPEAT,
            .wrap_t=GL_REPEAT,
            .min_filter=filter_type,
            .mag_filter=filter_type
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
    unsigned int mag_filter = this->sim_tex_params.mag_filter;
    unsigned int min_filter = this->sim_tex_params.min_filter;
    this->coefficients.reset(
        {
            .format=GL_RG32F, 
            // .width=(uint32_t)rectangular_root(n_states)[0],
            // .height=(uint32_t)rectangular_root(n_states)[1],
            .width=(uint32_t)get_closest_larger_square(n_states)[0],
            .height=(uint32_t)get_closest_larger_square(n_states)[1],
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=min_filter,
            .mag_filter=mag_filter,
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
            .min_filter=min_filter,
            .mag_filter=mag_filter,
        }
    ),
    this->energies.reset(
        {
            .format=GL_RG32F,
            .width=(uint32_t)n_states, .height=1,
            .wrap_s=GL_REPEAT,
            .wrap_t=GL_REPEAT,
            .min_filter=min_filter,
            .mag_filter=mag_filter
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

void Simulation::compute_new_energies(const sim_2d::SimParams &params) {
    if (params.gridWidth != this->m_potential.cols() ||
        params.gridHeight != this->m_potential.rows()) {
        m_wave_function = Eigen::VectorXcf(params.gridWidth*params.gridHeight);
        m_potential = Eigen::MatrixXd(params.gridHeight, params.gridWidth);
        // this->m_frames.
        m_frames.sim_tex_params.width = params.gridWidth;
        m_frames.sim_tex_params.height = params.gridHeight;
        m_frames.wave_func.reset(m_frames.sim_tex_params);
        m_frames.potential.reset(m_frames.sim_tex_params);
    }
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
        m_initial_coefficients[params.numberOfStates - 1] = 1.0;
    }
    init_energy_states(
        m_energy_eigenvalues, m_energy_eigenstates,
        m_potential,
        {
            .hbar=params.hbar, .m=params.m,
            .width=params.width, .height=params.height,
            .grid_width=params.gridWidth, .grid_height=params.gridHeight,
            .n_states=params.numberOfStates,
            .laplacian_stencil=params.laplacianStencil.selected,
            .global_shift=m_constant_potential
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
        if (i == params.numberOfStates - 3)
            m_initial_coefficients[i] 
                = (1.0 + std::complex<double>(0.0, 1.0))/sqrt(2.0);
        if (i == params.numberOfStates - 2)
            m_initial_coefficients[i]
                 = (1.0 + std::complex<double>(0.0, -1.0))/sqrt(2.0);
    }
    // std::complex<double> norm_factor
    //      = m_coefficients.adjoint()*m_coefficients;
    // for (int i = 0; i < params.numberOfStates; i++) {

    // }
    m_constant_potential = 0.0;
    m_potential = Eigen::MatrixXd(params.gridHeight, params.gridWidth);
    this->set_potential_from_string(
        params, "x^2+y^2", {});
}

void Simulation::use_double_slit_potential(const sim_2d::SimParams &params) {
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = ((double)j + 0.5)/(double)params.gridWidth - 0.5;
            double y = ((double)i + 0.5)/(double)params.gridHeight - 0.5;
            double val = 0.0;
            if (y > 0.0 && y < 0.07) // Make a partition
                val = 10.0;
            if (x > -0.17 && x < -0.12) // Cut the first slit on the partition
                val = 0.0;
            if (x > 0.12 && x < 0.17) // Second slit
                val = 0.0;
            m_potential(i, j) = val;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
}

void Simulation::use_heart_potential(const sim_2d::SimParams &params) {
    double size = 1.4;
    double height = 10.0;
    double edge_sharpness = 50.0;
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = ((double)j + 0.5)/(double)params.gridWidth - 0.5;
            double y = ((double)i + 0.5)/(double)params.gridHeight - 0.7;
            double r = 5.0*sqrt(x*x + y*y);
            double angle = std::arg(std::complex<double>(y, x));
            double pi = 3.141592653589793;
            angle = (angle < 0.0)? angle + 2.0*pi: angle;
            double s = size*(abs(sin(angle)) + 2.0*exp(-1.2*abs(angle - pi)));
            double val = height*(tanh(edge_sharpness*(r - s))/2.0 + 0.5); 
            m_potential(i, j) = val;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
}

void Simulation::use_triangle_potential(const sim_2d::SimParams &params) {
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = ((double)j + 0.5)/(double)params.gridWidth - 0.5;
            double y = ((double)i + 0.5)/(double)params.gridHeight - 0.5;
            double l1 = -1.5*x + 0.4;
            double l2 = 1.5*x + 0.4;
            double l3 = -0.32;
            if (y < l1 && y < l2 && y > l3)
                m_potential(i, j) = 0.0;
            else
                m_potential(i, j) = 10.0;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
}

void Simulation::use_pentagon_potential(const sim_2d::SimParams &params) {
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x 
                = (1.0/0.9)*((double)j + 0.5)/(double)params.gridWidth - 0.05;
            double y
                = (1.0/0.9)*((double)i + 0.5)/(double)params.gridHeight - 0.05;
            double l1 = x + 0.5;
            double l2 = -x + 3.0/2.0;
            double l3 = -2.0*x + 0.5;
            double l4 = 2.0*x - 3.0/2.0;
            if (y < l1 && y < l2 && y > l3 && y > l4 
                && x > 0.0 && x < 1.0 && y < 1.0 && y > 0.0)
                m_potential(i, j) = 0.0;
            else
                m_potential(i, j) = 10.0;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
}

void Simulation::use_hexagon_potential(const sim_2d::SimParams &params) {
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x 
                = (1.0/0.9)*((double)j + 0.5)/(double)params.gridWidth - 0.05;
            double y
                = (1.0/0.9)*((double)i + 0.5)/(double)params.gridHeight - 0.05;
            double l1 = 3.0*x/2.0 + 0.5;
            double l2 = -3.0*x/2.0 + 2.0;
            double l3 = -3.0*x/2.0 + 0.5;
            double l4 = 3.0*x/2.0 - 1.0;
            if (y < l1 && y < l2 && y > l3 && y > l4 
                && x > 0.0 && x < 1.0 && y < 1.0 && y > 0.0)
                m_potential(i, j) = 0.0;
            else
                m_potential(i, j) = 10.0;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
}

void Simulation::use_window_potential(const sim_2d::SimParams &params) {
     for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = ((double)j + 0.5)/(double)params.gridWidth;
            double y = ((double)i + 0.5)/(double)params.gridHeight;
            double val;
            if ((x > 0.1 && x < 0.45 && y > 0.1 && y < 0.45) 
                || (x > 0.55 && x < 0.9 && y > 0.1 && y < 0.45) 
                || (x > 0.55 && x < 0.9 && y > 0.55 && y < 0.9) 
                || (x > 0.1 && x < 0.45 && y > 0.55 && y < 0.9)
                ) {
                val = 0.0;
            } else {
                val = 4.0;
            }
            m_potential(i, j) = val;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
}

void Simulation::use_octagon_potential(const sim_2d::SimParams &params) {
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x 
                = (1.0/0.9)*((double)j + 0.5)/(double)params.gridWidth - 0.05;
            double y
                = (1.0/0.9)*((double)i + 0.5)/(double)params.gridHeight - 0.05;
            double l1 = x + 2.0/3.0;
            double l2 = -x + 5.0/3.0;
            double l3 = x - 2.0/3.0;
            double l4 = -x + 1.0/3.0;
            if (y < l1 && y < l2 && y > l3 && y > l4 
                && x > 0.0 && x < 1.0 && y < 1.0 && y > 0.0)
                m_potential(i, j) = 0.0;
            else
                m_potential(i, j) = 10.0;
        }
    }
    this->update_potential_tex();
    this->compute_new_energies(params);
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

void Simulation::update_potential_tex() {
    std::vector<Vec2> tmp(
        m_frames.sim_tex_params.width*m_frames.sim_tex_params.height);
    for (int i = 0; i < m_frames.sim_tex_params.height; i++)
        for (int j = 0; j < m_frames.sim_tex_params.width; j++)
            tmp[i*m_frames.sim_tex_params.width + j] 
                = {.x=(float)m_potential(i, j), .y=0.0};
    this->m_frames.potential.set_pixels((float *)&tmp[0]);
}

std::map<std::string, double> Simulation::set_potential_from_string(
    const sim_2d::SimParams &params, const std::string &string_val,
    std::map<std::string, double> user_params) {
    std::vector<std::string> expr_stack = get_expression_stack(string_val);
    std::vector<std::string> rpn_list = shunting_yard(expr_stack);
    std::set<std::string> variables 
        = get_variables_from_rpn_list(rpn_list);
    std::map<std::string, double> variables_values;
    for (auto &variable: variables)
        variables_values.insert({variable, 1.0F});
    for (auto &e: user_params)
        if (variables_values.count(e.first))
            variables_values.insert_or_assign(e.first, e.second);
    std::map<std::string, double> ret_val {};
    for (auto &e: variables_values)
        ret_val.insert(e);
    double min_val = 0.0;
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            double x = params.width 
                * (((double)j + 0.5)/(double)params.gridWidth - 0.5);
            double y = params.height
                 * (((double)i + 0.5)/(double)params.gridHeight - 0.5);
            variables_values.insert_or_assign("x", x);
            variables_values.insert_or_assign("y", y);
            double val = compute_expression(rpn_list, variables_values);
            min_val = (i == 0 && j == 0)? val: ((val < min_val)? val: min_val);
            m_potential(i, j) = val;
        }
    }
    if (min_val < 0.0)
        this->m_constant_potential = abs(min_val);
    this->compute_new_energies(params);
    this->update_potential_tex();
    return ret_val;
}

void Simulation::set_potential_from_image(
    const sim_2d::SimParams &params, 
    const uint8_t *image_data, IVec2 image_dimensions) { 
    int w = image_dimensions[0];
    int h = image_dimensions[1];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int pix_ind = i*w + j;
            double r = (double)image_data[4*pix_ind];
            double g = (double)image_data[4*pix_ind + 1];
            double b = (double)image_data[4*pix_ind + 2];
            double mono_val = sqrt(r*r + g*g + b*b)/8.0;
            if (i < params.gridWidth && j < params.gridHeight)
                m_potential(params.gridHeight - i - 1, j) = mono_val;
        }
    }
    this->compute_new_energies(params);
    this->update_potential_tex();
}

Simulation::Simulation(int window_width, int window_height, 
                       unsigned int filter_type, sim_2d::SimParams params):
    m_programs(),
    m_frames(
        window_width, window_height, 
        params.gridWidth, params.gridHeight, 
        filter_type, params.numberOfStates) {
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
        energy -= m_constant_potential;
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

void Simulation::set_preset_potential(
    const sim_2d::SimParams &params, const std::string &val
) {
    if (val == "Circle" || val == "circle" || val == "Finite circular well")
        this->set_potential_from_string(
            params, "5.0*step(sqrt(x^2 + y^2) - 4.0)", {});
    else if (val == "Heart" || val == "heart" || val == "Heart well")
        this->use_heart_potential(params);
    else if (val == "Barrier with slits" || val == "Double slit")
        this->use_double_slit_potential(params);
    else if (val == "Triangle")
        this->use_triangle_potential(params);
    else if (val == "Pentagon")
        this->use_pentagon_potential(params);
    else if (val == "Hexagon")
        this->use_hexagon_potential(params);
    else if (val == "Octagon")
        this->use_octagon_potential(params);
    else if (val == "Four overlapping wells")
        this->set_potential_from_string(params,
            "-18*("
            " exp(-0.5*((x+2.5)^2+y^2)/1.45)"
            "+exp(-0.5*((x-2.5)^2+y^2)/1.45)"
            "+exp(-0.5*(x^2+(y-2.5)^2)/1.45)"
            "+exp(-0.5*(x^2+(y+2.5)^2)/1.45)"
            ")",
            {});
    else if (val == "Window")
        this->use_window_potential(params);
    else
        this->set_potential_from_string(params, val, {});

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
        uint32_t program = (params.colorPhase)? 
            m_programs.surface_domain_color: m_programs.surface_mag_color_map;
        Uniforms vertex_uniforms = {
            {"heightTex", &m_frames.wave_func},
            {"rotation", rotation},
            {"screenDimensions", screen_dimensions},
            {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
            {"heightScale", params.heightScales[0]/1000.0F},
            {"scale", scale},
            {"dimensions2D", 
                IVec2{.ind{512, 512}}},
            {"heightDataType", int(1)}
        };
        Uniforms domain_color_uniforms {
            {"tex", &m_frames.wave_func},
            {"brightness", params.brightness},
        };
        Uniforms mag_color_map_uniforms {
            {"tex", &m_frames.wave_func},
            {"brightness", params.brightness},
            {"color", Vec3{.r=1.0, .g=1.0, .b=1.0}},
        };
        Uniforms potential_single_color_uniforms {
            {"color", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=1.0}},
        };
        for (const auto &e: vertex_uniforms) {
            domain_color_uniforms.insert(e);
            mag_color_map_uniforms.insert(e);
            potential_single_color_uniforms.insert(e);
        }
        m_frames.main_render.draw(
            program,
            (params.colorPhase)?
                domain_color_uniforms: mag_color_map_uniforms,
            m_frames.surface,
            Config::viewport(
                0, 0, 
                m_frames.main_view_tex_params.height, 
                m_frames.main_view_tex_params.height)
        );
        if (params.heightScales[1] > 0.0) { 
            potential_single_color_uniforms["heightScale"] 
                = params.heightScales[1]/500.0F;
            potential_single_color_uniforms["heightTex"] 
                = &m_frames.potential;
            potential_single_color_uniforms["heightDataType"] = 0;
            m_frames.main_render.draw(
                m_programs.surface_single_color,
                potential_single_color_uniforms,
                m_frames.surface,
                Config::viewport(
                    0, 0, 
                    m_frames.main_view_tex_params.height, 
                    m_frames.main_view_tex_params.height)
            );
        }
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        uint32_t program = (params.colorPhase)?
            m_programs.domain_color: m_programs.mag_color_map;
        Uniforms domain_color_uniforms = {
            {"tex", &m_frames.wave_func},
            {"brightness", float(params.brightness)},
            {"encodeMagAsBrightness", 1},
            {"index", 0}
        };
        Uniforms mag_color_map_uniforms = {
            {"tex", &m_frames.wave_func},
            {"brightness", float(params.brightness)},
            {"color", Vec3{.r=1.0, .g=1.0, .b=1.0}},
            {"index", 0}
        };
        m_frames.main_render.draw(
            program,
            (params.colorPhase)?
                domain_color_uniforms: mag_color_map_uniforms,
            quad_wire_frame,
            Config::viewport(
                0, 0, 
                m_frames.main_view_tex_params.height, 
                m_frames.main_view_tex_params.height)
        );
        if (params.brightness2 > 0.0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            m_frames.main_render.draw(
                m_programs.mag_color_map,
                {
                    {"tex", &m_frames.potential},
                    {"brightness", float(params.brightness2)},
                    {"color", Vec3{.r=1.0, .g=1.0, .b=1.0}}
                },
                quad_wire_frame,
                Config::viewport(
                    0, 0, 
                    m_frames.main_view_tex_params.height, 
                    m_frames.main_view_tex_params.height)
            );
            glDisable(GL_BLEND);
        }
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