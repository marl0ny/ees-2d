#include <math.h>
#include "params.hpp"

#include <Eigen/Core>


#pragma once

using namespace Eigen;


MatrixXd init_potential(PotentialParams &potential_params,
                        size_t width, size_t height);

