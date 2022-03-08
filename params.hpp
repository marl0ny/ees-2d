#include "gl_wrappers2d/gl_wrappers.hpp"

#pragma once

// #define PIXEL_START_WIDTH 64
// #define PIXEL_START_HEIGHT 64
// #define SIM_START_WIDTH 22.75/2.0
// #define SIM_START_HEIGHT 22.75/2.0
// #define PIXEL_START_WIDTH 128
// #define PIXEL_START_HEIGHT 128
// #define SIM_START_WIDTH 22.75
// #define SIM_START_HEIGHT 22.75
#define PIXEL_START_WIDTH 256
#define PIXEL_START_HEIGHT 256
#define SIM_START_WIDTH 45.5
#define SIM_START_HEIGHT 45.5

struct ViewParams {
    int width = PIXEL_START_WIDTH;
    int height = PIXEL_START_HEIGHT;
    float view_ratio = 1.25;
};


struct SimParams {
    float width = SIM_START_WIDTH;
    float height = SIM_START_HEIGHT;
    float dx = SIM_START_WIDTH/PIXEL_START_WIDTH;
    float dy = SIM_START_HEIGHT/PIXEL_START_HEIGHT;
    float dt = 0.01;
    float m = 1.0;
    float hbar = 1.0;
    float r_scale_v = 0.0;
    int laplace_points = 5;
};


struct PotentialParams {
    enum potential_types {
        SHO = 1,
        DOUBLE_SLIT, SINGLE_SLIT,
        STEP, INV_R, TRIPLE_SLIT,
        NEG_INV_R, CIRCLE, CONE
    };
    float a = 10.0;
    float y0 = 0.5;
    float w = 0.03;
    float spacing = 0.05;
    float x1 = 0.45;
    float x2 = 0.55;
    float a_imag = 0.0;
    int potential_type = 0;
};


struct ViewFrameReads {
    Quad *frame1 = nullptr;
    Quad *frame2 = nullptr;
    Quad *frame3 = nullptr;
    Quad *frame_potential = nullptr;
    Quad *frame_vec = nullptr;
    Quad *frame_text = nullptr;
    Quad *frame_background = nullptr;
};

