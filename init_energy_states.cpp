#include "init_energy_states.hpp"

void init_energy_states(SparseMatrix<double> &mat, 
                        std::vector<Triplet<double>> &triplets,
                        VectorXcd &eigenvalues, MatrixXcd &eigenvectors,
                        int n_states,
                        const MatrixXd &potential, 
                        const SimParams &sim_params,
                        size_t width, size_t height) {
    double dx = sim_params.dx;
    double dy = sim_params.dy;
    double hbar = sim_params.hbar;
    double m = sim_params.m;
    // int laplace_points = sim_params.laplace_points;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // double y = (double)((double)i - (double)height/2.0)/(double)height;
            // double x = (double)((double)j - (double)width/2.0)/(double)width;
            double u = 1.0/(dy*dy), d = 1.0/(dy*dy);
            double l = 1.0/(dx*dx), r = 1.0/(dx*dx);
            double c = -2.0/(dx*dx) - 2.0/(dy*dy);
            double potential_val = potential(i, j);
            triplets.push_back(Triplet<double>(i*width + j, i*width + j, 
                                               - hbar*hbar*c/(2.0*m)
                                               + potential_val));
            if (i < height-1) triplets.push_back(
                Triplet<double>(i*width + j, (i + 1)*width + j,
                                -hbar*hbar*u/(2.0*m)));
            if (i > 0) triplets.push_back(
                Triplet<double>(i*width + j, (i - 1)*width + j,
                                -hbar*hbar*d/(2.0*m)));
            if (j > 0) triplets.push_back(
                Triplet<double>(i*width + j, i*width + (j - 1),
                                -hbar*hbar*l/(2.0*m)));
            if (j < width-1) triplets.push_back(
                Triplet<double>(i*width + j, i*width + (j + 1),
                                -hbar*hbar*r/(2.0*m)));
        }
    }
    mat.setFromTriplets(triplets.begin(), triplets.end());
    mat.makeCompressed();
    SparseSymShiftSolve<double> op(mat);
    SymEigsShiftSolver<SparseSymShiftSolve<double>> eigs(op, n_states, 
                                                         n_states + 10, 0.0);
    eigs.init();
    int nconv = eigs.compute(SortRule::LargestMagn);

    std::cout << nconv << std::endl;
    if (eigs.info() == CompInfo::Successful) {
        eigenvalues = eigs.eigenvalues();
        eigenvectors = eigs.eigenvectors();
    } else {
        fprintf(stderr, "Computation failed\n");
        // exit(1);
    }
}

// # # # # #
//  # # # #
// # # # #