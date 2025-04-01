#include <complex>
#include <vector>
#include <iostream>

#include <stdlib.h>

#include "params.hpp"

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/SparseGenMatProd.h>

using namespace Eigen;
using namespace Spectra;
using complex = std::complex<double>;

#pragma once

void init_energy_states(SparseMatrix<double> &mat, 
                        std::vector<Triplet<double>> &triplets,
                        VectorXcd &eigenvalues, MatrixXcd &eigenvectors,
                        int n_states, 
                        const MatrixXd &potential, 
                        const SimParams &sim_params,
                        size_t width, size_t height);
