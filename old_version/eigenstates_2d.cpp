/*
Finding the energy eigenstates for a single particle 2D system with 
Dirichlet boundary conditions, 
by discretizing the Hamiltonian using finite differences.

This requires that you have Eigen, Spectra, and SDL installed.

Eigen: https://eigen.tuxfamily.org/index.php?title=Main_Page
Spectra: https://spectralib.org/
SDL: https://www.libsdl.org/

Compile with:
 clang++ -c eigenstates_2d.cpp -std=c++17 -O3 -Wall -g
 clang++ -std=c++17 -O3 -Wall -g -o program eigenstates_2d.o -lSDL2
You will need to also need to specify the path 
to Eigen, Spectra, and SDL as well.

*/

#include <complex>
#include <vector>
#include <iostream>

#include <stdlib.h>

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/SparseGenMatProd.h>

#include <SDL2/SDL.h>


using namespace Eigen;
using namespace Spectra;
using complex = std::complex<double>;

const size_t size = 256;
const size_t matrix_size = size*size;

const int VIEW_WIDTH = 512;
const int VIEW_HEIGHT = 512;

enum potential_type {
    SHO=0, ISW, CIRCLE
};
const int n_types = 3;


struct Colour {
    int red;
    int green;
    int blue;
};


void complex_to_colour(struct Colour *col, complex number) {
    double pi = 3.141592653589793;
    double arg_val = arg(number);
    int max_col = 255;
    int min_col = 50;
    int col_range = max_col - min_col;
    if (arg_val <= pi/3 && arg_val >= 0) {
        col->red = max_col;
        col->green = min_col + (int)(col_range*arg_val/(pi/3));
        col->blue = min_col;
    } else if (arg_val > pi/3 && arg_val <= 2*pi/3){
        col->red = max_col - (int)(col_range*(arg_val - pi/3)/(pi/3));
        col->green = max_col;
        col->blue = min_col;
    } else if (arg_val > 2*pi/3 && arg_val <= pi){
        col->red = min_col;
        col->green = max_col;
        col->blue = min_col + (int)(col_range*(arg_val - 2*pi/3)/(pi/3));
    } else if (arg_val < 0.0 && arg_val > -pi/3){
        col->red = max_col;
        col->blue = min_col - (int)(col_range*arg_val/(pi/3));
        col->green = min_col;
    } else if (arg_val <= -pi/3 && arg_val > -2*pi/3){
        col->red = max_col + (int)(col_range*(arg_val + pi/3)/(pi/3));
        col->blue = max_col;
        col->green = min_col;
    } else if (arg_val <= -2*pi/3 && arg_val >= -pi){
        col->red = min_col;
        col->blue = max_col;
        col->green = min_col - (int)(col_range*(arg_val + 2*pi/3)/(pi/3));
    }
    else {
        col->red = min_col;
        col->green = max_col;
        col->blue = max_col;
    }
}


void init_energy_states(VectorXcd &eigenvalues, MatrixXcd &eigenvectors, 
                        int potential_type, int n_states) {
    std::vector<Triplet<double> > triplets;
    triplets.reserve(size*size*5);
    double dx = 1.0/size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double y = (double)((double)i - (double)size/2.0)/(double)size;
            double x = (double)((double)j - (double)size/2.0)/(double)size;
            double u = 1.0/(dx*dx), d = 1.0/(dx*dx);
            double l = 1.0/(dx*dx), r = 1.0/(dx*dx);
            double c = -4.0/(dx*dx);
            double potential = 0.0;
            if (potential_type == potential_type::SHO) {
                potential = 10000.0*(x*x + y*y);
            } else if (potential_type == potential_type::ISW) {
                potential = 0.0;
            } else if (potential_type == potential_type::CIRCLE) {
                potential = (sqrt(x*x + y*y) > 0.35)? 5000.0: 0.0;
            }
            triplets.push_back(Triplet<double>(i*size + j, i*size + j, 
                                               -0.5*c + potential));
            if (i < size-1) triplets.push_back(
                Triplet<double>(i*size + j, (i + 1)*size + j, -0.5*u));
            if (i > 0) triplets.push_back(
                Triplet<double>(i*size + j, (i - 1)*size + j, -0.5*d));
            if (j > 0) triplets.push_back(
                Triplet<double>(i*size + j, i*size + (j - 1), -0.5*l));
            if (j < size-1) triplets.push_back(
                Triplet<double>(i*size + j, i*size + (j + 1), -0.5*r));
        }
    }
    SparseMatrix<double> mat(matrix_size, matrix_size);
    mat.reserve(VectorXi::Constant(matrix_size, 5));
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
        exit(1);
    }
}


int main() {

    VectorXcd eigenvalues;
    MatrixXcd eigenvectors;
    int total = 50;
    init_energy_states(eigenvalues, eigenvectors, potential_type::CIRCLE, total);
    std::cout << eigenvalues << std::endl;

    SDL_Window* window = NULL;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to initialize SDL2: %s\n", SDL_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("Window",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              VIEW_WIDTH, VIEW_HEIGHT,
                              SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
    int window_width = VIEW_WIDTH;
    int window_height = VIEW_HEIGHT;
    SDL_Rect background_rect = {.x = 0, .y = 0, 
                                .w = window_width, .h = window_height};
    SDL_SetRenderDrawColor(renderer, 9, 0, 0, 255);
    SDL_RenderClear(renderer);

    const Uint8 *keyboard_state;
    int n = 0;
    int selected_potential = 0;
    int returned = 0;
    int delay_time = 15;
    int key_state = 0;
    double scale = 1.0;
    double t = 0.0;
    do {
        t += 0.0001;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_GetWindowSize(window, &window_width, &window_height);
        background_rect.w = VIEW_WIDTH;
        background_rect.h = VIEW_HEIGHT;
        SDL_RenderFillRect(renderer, &background_rect);
        SDL_PumpEvents();
        keyboard_state = SDL_GetKeyboardState(NULL);
        if (keyboard_state[SDL_SCANCODE_RETURN]) {
            returned = 1;
        }
        int key_pressed = 0;
        if (keyboard_state[SDL_SCANCODE_UP]) {
            scale += 1.0;
            key_pressed = 1;
            key_state = SDL_SCANCODE_UP;
        }
        if (keyboard_state[SDL_SCANCODE_DOWN]) {
            scale -= 1.0;
            key_pressed = 1;
            key_state = SDL_SCANCODE_DOWN;
        }
        if (keyboard_state[SDL_SCANCODE_LEFT]) {
            key_pressed = 1;
            key_state = SDL_SCANCODE_LEFT;
        }
        if (keyboard_state[SDL_SCANCODE_RIGHT]) {
            key_pressed = 1;
            key_state = SDL_SCANCODE_RIGHT;
        }
        if (keyboard_state[SDL_SCANCODE_SPACE]) {
            key_pressed = 1;
            key_state = SDL_SCANCODE_SPACE;
        }
        if (key_pressed == 0) {
            if (key_state == SDL_SCANCODE_RIGHT) {
                n = (n - 1 < 0)? total - 1: n - 1;
            } else if (key_state == SDL_SCANCODE_LEFT) {
                n = (n + 1) % total;
            } else if (key_state == SDL_SCANCODE_SPACE) {
                selected_potential = (selected_potential + 1) % n_types;
                init_energy_states(eigenvalues, eigenvectors, 
                                   selected_potential, total);
            }
            key_state = 0;
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                Colour colour;
                auto phase = exp(-complex(0.0, 1.0)*t*eigenvalues(n));
                complex_to_colour(&colour, 
                                  1000.0*eigenvectors(i*size + j, n)
                                  *phase);
                double real = eigenvectors(i*size + j, n).real();
                double imag = eigenvectors(i*size + j, n).imag();
                double abs2_val = abs(scale*50000.0*(real*real + imag*imag));
                SDL_SetRenderDrawColor(renderer, 
                                       std::min(255, (int)abs2_val*colour.red), 
                                       std::min(255, (int)abs2_val*colour.green), 
                                       std::min(255, (int)abs2_val*colour.blue), 255
                                       );
                SDL_Rect rect;
                rect.x = i*2;
                rect.y = j*2;
                rect.w = 2;
                rect.h = 2;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(delay_time);
    } while (returned == 0);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}