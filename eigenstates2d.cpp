#include "eigenstates2d.hpp"
#include <complex>
#include <vector>
#include <iostream>

#include <stdlib.h>


using namespace Eigen;
using namespace Spectra;
using complex = std::complex<double>;

static void construct_hamiltonian_2nd_order_5_pt(
    std::vector<Triplet<double> > &triplets,
    const MatrixXd &potential,
    InitEnergyStatesParams params) {
    triplets.reserve(params.grid_width*params.grid_height*5);
    double dx = params.width/double(params.grid_width);
    double dy = params.height/double(params.grid_height);
    double hbar = params.hbar;
    int grid_height = params.grid_height;
    int grid_width = params.grid_width;
    double m = params.m;
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            double u = (-hbar*hbar/(2.0*m))/(dy*dy);
            double d = (-hbar*hbar/(2.0*m))/(dy*dy);
            double l = (-hbar*hbar/(2.0*m))/(dx*dx);
            double r = (-hbar*hbar/(2.0*m))/(dx*dx);
            double c = (-hbar*hbar/(2.0*m))*(-2.0/(dx*dx) - 2.0/(dy*dy));
            triplets.push_back(Triplet<double>(i*grid_width + j, 
                                               i*grid_width + j, 
                                               c + potential(i, j)));
            if (i < grid_height-1) triplets.push_back(
                Triplet<double>(i*grid_width + j, (i + 1)*grid_width + j, u));
            if (i > 0) triplets.push_back(
                Triplet<double>(i*grid_width + j, (i - 1)*grid_width + j, d));
            if (j > 0) triplets.push_back(
                Triplet<double>(i*grid_width + j, i*grid_width + (j - 1), l));
            if (j < grid_width-1) triplets.push_back(
                Triplet<double>(i*grid_width + j, i*grid_width + (j + 1), r));
        }
    }
}

static void setAt(
    std::vector<Triplet<double>> &triplets,
    int grid_width,
    int index1, int index2, int index3, int index4, double value) {
    triplets.push_back(
        Triplet<double>(
            index1*grid_width + index2, 
            index3*grid_width + index4, value));
}

static void construct_hamiltonian_4th_order_9_pt(
    std::vector<Triplet<double> > &triplets,
    const MatrixXd &potential,
    InitEnergyStatesParams params) {
    triplets.reserve(params.grid_width*params.grid_height*9);
    double dx = params.width/double(params.grid_width);
    double dy = params.height/double(params.grid_height);
    double hbar = params.hbar;
    int grid_height = params.grid_height;
    int grid_width = params.grid_width;
    double m = params.m;
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            double c0 = 0.0;
            // if (j >= 2 && j < grid_width - 2) {
            double l2 = (-hbar*hbar/(2.0*m))*(-1.0/12.0)/(dx*dx);
            double l1 = (-hbar*hbar/(2.0*m))*(4.0/3.0)/(dx*dx);
            double r2 = (-hbar*hbar/(2.0*m))*(-1.0/12.0)/(dx*dx);
            double r1 = (-hbar*hbar/(2.0*m))*(4.0/3.0)/(dx*dx);
            c0 += (-hbar*hbar/(2.0*m))*(-5.0/2.0)/(dx*dx);
            c0 += potential(i, j);
            if (j >= 2)
                setAt(triplets, grid_width,
                    i, j, i, j-2, l2);
            if (j >= 1)
                setAt(triplets, grid_width,
                    i, j, i, j-1, l1);
            if (j < grid_width - 1)
                setAt(triplets, grid_width, 
                    i, j, i, j+1, r1);
            if (j < grid_width - 2)
                setAt(triplets, grid_width, 
                    i, j, i, j+2, r2);
            // if (i >= 2 && i < grid_height - 2) {
            double u2 = (-hbar*hbar/(2.0*m))*(-1.0/12.0)/(dy*dy);
            double u1 = (-hbar*hbar/(2.0*m))*(4.0/3.0)/(dy*dy);
            double d1 = (-hbar*hbar/(2.0*m))*(4.0/3.0)/(dy*dy);
            double d2 = (-hbar*hbar/(2.0*m))*(-1.0/12.0)/(dy*dy);
            c0 += (-hbar*hbar/(2.0*m))*(-5.0/2.0)*(1.0/(dy*dy));
            if (i >= 2)
                setAt(triplets, grid_width, 
                    i, j, i-2, j, u2);
            if (i >= 1)
                setAt(triplets, grid_width, 
                    i, j, i-1, j, u1);
            if (i < grid_height - 1)
                setAt(triplets, grid_width, 
                    i, j, i+1, j, d1);
            if (i < grid_height - 2)
                setAt(triplets, grid_width, 
                    i, j, i+2, j, d2);
            setAt(triplets, grid_width, 
                i, j, i, j, c0);
        }
    }
}

void init_energy_states(VectorXcd &eigenvalues, MatrixXcd &eigenvectors, 
                        const MatrixXd &potential,
                        InitEnergyStatesParams params) {
    size_t matrix_size = params.grid_width*params.grid_height;
    std::vector<Triplet<double> > triplets;
    int reserve_size;
    switch (params.laplacian_stencil) {
        case InitEnergyStatesParams::LAPLACIAN_2ND_OR_5_PT:
        reserve_size = 5;
        construct_hamiltonian_2nd_order_5_pt(triplets, potential, params);
        break;
        case InitEnergyStatesParams::LAPLACIAN_4TH_OR_9_PT:
        reserve_size = 9;
        construct_hamiltonian_4th_order_9_pt(triplets, potential, params);
        break;
        default:
        break;
    }
    SparseMatrix<double> mat(matrix_size, matrix_size);
    mat.reserve(VectorXi::Constant(matrix_size, reserve_size));
    mat.setFromTriplets(triplets.begin(), triplets.end());
    mat.makeCompressed();
    SparseSymShiftSolve<double> op(mat);
    SymEigsShiftSolver<SparseSymShiftSolve<double>> eigs(op, params.n_states, 
                                                         params.n_states + 10, 
                                                         0.0);
    eigs.init();
    int nconv = eigs.compute(SortRule::LargestMagn);

    std::cout << nconv << std::endl;
    if (eigs.info() == CompInfo::Successful) {
        eigenvalues = eigs.eigenvalues();
        eigenvectors = eigs.eigenvectors();
    } else {
        fprintf(stderr, "Unable to compute eigenvalues and eigenvectors.\n");
    }
}
