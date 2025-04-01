#ifndef _PRESET_POTENTIAL_
#define _PRESET_POTENTIAL_


#include <Eigen/Core>

using namespace Eigen;

struct PotentialParams {
    enum potential_types {
        EMBEDDED_HARMONIC = 1,
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
    int potential_type = EMBEDDED_HARMONIC;
    int width, height;
};


MatrixXd init_potential(const PotentialParams &potential_params);

#endif
