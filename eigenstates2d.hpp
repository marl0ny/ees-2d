#ifndef _EIGENSTATES2D_
#define _EIGENSTATES2D_

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/SparseGenMatProd.h>


struct InitEnergyStatesParams {
    enum {
        LAPLACIAN_2ND_OR_5_PT=0, LAPLACIAN_4TH_OR_9_PT
    };
    double hbar, m;
    double width, height;
    int grid_width, grid_height;
    int n_states;
    int laplacian_stencil;
};

void init_energy_states(
    Eigen::VectorXcd &eigenvalues,
    Eigen::MatrixXcd &eigenvectors, 
    const Eigen::MatrixXd &potential,
    InitEnergyStatesParams params);


#endif