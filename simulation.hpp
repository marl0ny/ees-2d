#ifndef _SIMULATION_
#define _SIMULATION_

#include "parameters.hpp"
#include <Eigen/Core>

struct Frames {
    TextureParams main_view_tex_params;
    TextureParams sim_tex_params;
    RenderTarget main_render;
    Quad wave_func;
    Quad potential;
    Quad coefficients;
    Quad coefficients_tmp;
    Quad energies;
    WireFrame circles;
    WireFrame clock_hands;
    // WireFrame expected_energy_level;
    WireFrame levels;
    WireFrame surface;
    Frames(
        int window_width, int window_height,
        int sim_width, int sim_height, 
        int n_states);
    void reset(int sim_width, int sim_height);
    void reset(int n_states);
};

struct Programs {
    uint32_t domain_color;
    uint32_t mag_color_map;
    uint32_t uniform_color;
    uint32_t circles;
    uint32_t clock_hands;
    uint32_t levels;
    uint32_t flip_horizontal;
    uint32_t surface_domain_color;
    uint32_t surface_mag_color_map;
    uint32_t surface_single_color;
    Programs();
};

class Simulation {
    Programs m_programs;
    Frames m_frames;
    double m_constant_potential;
    Eigen::MatrixXd m_potential;
    Eigen::VectorXcf m_wave_function;
    Eigen::VectorXcd m_initial_coefficients;
    Eigen::VectorXcd m_coefficients;
    Eigen::VectorXcd m_energy_eigenvalues;
    Eigen::MatrixXcd m_energy_eigenstates;
    bool is_recomputing;
    void config_at_start(sim_2d::SimParams &params);
    void use_heart_potential(const sim_2d::SimParams &params);
    void use_double_slit_potential(const sim_2d::SimParams &params);
    void use_triangle_potential(const sim_2d::SimParams &params);
    void use_pentagon_potential(const sim_2d::SimParams &params);
    void use_octagon_potential(const sim_2d::SimParams &params);
    void use_hexagon_potential(const sim_2d::SimParams &params);
    void update_potential_tex();
    public:
    Simulation(int window_width, int window_height, sim_2d::SimParams params);
    void compute_new_energies(
        const sim_2d::SimParams &params);
    // void compute_new_
    void freeze();
    void time_step(sim_2d::SimParams &params);
    const RenderTarget &view(
        sim_2d::SimParams &params, ::Quaternion rotation, float scale);
    void modify_stationary_state_coefficient(
        sim_2d::SimParams &params, 
        Vec2 cursor_pos1, Vec2 cursor_pos2);
    void select_stationary_state_from_energy_levels(
        sim_2d::SimParams &params, Vec2 cursor_pos
    );
    void approximate_wavepacket(
        sim_2d::SimParams &params,
        Vec2 cursor_pos1, Vec2 cursor_pos2
    );
    void set_preset_potential(
        const sim_2d::SimParams &param, const std::string &val);
    std::map<std::string, double> set_potential_from_string(
        const sim_2d::SimParams &param,
        const std::string &val,
        std::map<std::string, double> user_params
    );
    void set_potential_from_image(
        const sim_2d::SimParams &param, const uint8_t *image_data,
        IVec2 image_dimensions
    );
    void normalize_wave_function(const sim_2d::SimParams &param);
    double measure_energy(const sim_2d::SimParams &params);
};

#endif