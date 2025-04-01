#include "preset_potential.hpp"


MatrixXd init_potential(const PotentialParams &potential_params) {
    double a = potential_params.a;
    double y0 = potential_params.y0;
    double w = potential_params.w;
    double spacing = potential_params.spacing;
    double x1 = potential_params.x1;
    double x2 = potential_params.x2;
    // double a_imag = potential_params.a_imag;
    int potential_type = potential_params.potential_type;
    int width = potential_params.width;
    int height = potential_params.height;
    MatrixXd potential(width, height);
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < height; i++) {
            double potential_val = 0.0;
            double y = 1.0 - ((double)i/height + 0.5/height);
            double x = (double)j/width + 0.5/width;
            // std::cout << x << ", " << y << std::endl;
            // potential_params.potential_types.
            double u, v, r, one_over_r;
            // std::cout << "\n" << PotentialParams::SHO << std::endl;
            switch(potential_type) {
                case PotentialParams::EMBEDDED_HARMONIC:
                potential_val = a*((x-0.5)*(x-0.5) + (y-0.5)*(y-0.5));
                break;
                case PotentialParams::DOUBLE_SLIT:
                if (y <= (y0 + w/2.0) &&
                    y >= (y0 - w/2.0) &&
                    (x <= x1 - spacing/2.0 ||
                    (x >= x1 + spacing/2.0 &&
                    x <= x2 - spacing/2.0
                    ) || x >= x2 + spacing/2.0
                    )) {
                    potential_val = a;
                }
                break;
                case PotentialParams::SINGLE_SLIT:
                if (y <= (y0 + w/2.0) &&
                    y >= (y0 - w/2.0) &&
                    (x <= x1 - spacing/2.0 ||
                    x >= x1 + spacing/2.0)) {
                    potential_val = a;
                }
                break;
                case PotentialParams::STEP:
                if (y > y0) {
                    potential_val = a;
                }
                break;
                case PotentialParams::INV_R:
                u = 10.0*(x - 0.5);        
                v = 10.0*(y - 0.5);
                one_over_r = 1.0/std::sqrt(u*u + v*v);
                potential_val = (one_over_r < 50.0)? one_over_r: 50.0;
                break;
                case PotentialParams::TRIPLE_SLIT:
                if ((y <= 0.45 || y >= 0.48) || (x > 0.49 && x < 0.51)
                    || (x > 0.43 && x < 0.45) || (x > 0.55 && x < 0.57)) {
                    potential_val = 0.0;
                } else {
                    potential_val = 15.0;
                }
                break;
                case PotentialParams::NEG_INV_R:
                u = 2.0*(x - 0.5);        
                v = 2.0*(y - 0.5);
                one_over_r = -1.0/std::sqrt(u*u + v*v);
                potential_val = (one_over_r < -150.0)? -150.0: one_over_r;
                break;
                case PotentialParams::CIRCLE:
                u = x - 0.5;
                v = y - 0.5;
                r = std::sqrt(u*u + v*v);
                potential_val = a*((r < spacing)? 0.0: 1.0);
                break;
                case PotentialParams::CONE:
                u = x - 0.5;
                v = y - 0.5;
                r = std::sqrt(u*u + v*v);
                potential_val = a*r;
                default:
                break;
            }
            potential(i, j) = potential_val;
        }
    }
    return potential;
}
