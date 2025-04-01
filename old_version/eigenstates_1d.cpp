/*
Finding the energy eigenstates for a single particle 1D system with 
Dirichlet boundary conditions, 
by discretizing the Hamiltonian using finite differences.

This requires that you have Eigen, Spectra, and SDL installed.

Eigen: https://eigen.tuxfamily.org/index.php?title=Main_Page
Spectra: https://spectralib.org/
SDL: https://www.libsdl.org/

Compile with:
 clang++ -c eigenstates_1d.cpp -std=c++17 -O3 -Wall -g
 clang++ -std=c++17 -O3 -Wall -g -o program eigenstates_1d.o -lSDL2
You will need to also need to specify the path to Eigen, Spectra, and SDL as well.

*/

#include <complex>
#include <iostream>
#include <stdlib.h>

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/SparseGenMatProd.h>

#include <SDL2/SDL.h>


#define WIDTH 640
#define HEIGHT 480


using namespace Eigen;
using namespace Spectra;
using complex = std::complex<double>;

int main() {
    const size_t size = 256;
    float dx = 1.0/size;
    SparseMatrix <double> mat(size, size);
    mat.reserve(VectorXi::Constant(size, 3));
    for (int i = 0; i < size; i++) {
        double x = (double)((double)i - (double)size/2.0)/(double)size;
        if (i < size-1) mat.insert(i, i+1) = -0.5/(dx*dx);
        mat.insert(i, i) = 1.0/(dx*dx) + 15000.0*x*x;
        if (i >= 1) mat.insert(i, i-1) = -0.5/(dx*dx);
    }
    mat.makeCompressed();
    int total = 50;

    // SparseGenMatProd<double> op(mat);
    // SymEigsSolver<SparseGenMatProd<double>> eigs(op, total, total*10);
    // eigs.init();
    // int nconv = eigs.compute(SortRule::SmallestMagn);

    SparseSymShiftSolve<double> op(mat);
    SymEigsShiftSolver<SparseSymShiftSolve<double>> eigs(op, total, total + 10, 0.0);
    eigs.init();
    int nconv = eigs.compute(SortRule::LargestMagn);

    std::cout << nconv << std::endl;
    VectorXcd eigenvalues;
    MatrixXcd eigenvectors;
    if (eigs.info() == CompInfo::Successful) {
        eigenvalues = eigs.eigenvalues();
        eigenvectors = eigs.eigenvectors();
    } else {
        fprintf(stderr, "Computation failed\n");
        exit(1);
    }
    std::cout << eigenvalues << std::endl;


    SDL_Window* window = NULL;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to initialize SDL2: %s\n", SDL_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("Window",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WIDTH, HEIGHT,
                              SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
    int window_width = WIDTH;
    int window_height = HEIGHT;
    SDL_Rect background_rect = {.x = 0, .y = 0, .w = window_width, .h = window_height};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);


    const Uint8 *keyboard_state;
    // Point p;
    int n = total - 1;
    int returned = 0;
    // int mouse_pressed = 0;
    // int mouse_released = 0;
    // int mouse_x = 0;
    // int mouse_y = 0;
    int delay_time = 15;
    int key_state = 0;
    do {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_GetWindowSize(window, &window_width, &window_height);
        background_rect.w = window_width;
        background_rect.h = window_height;
        SDL_RenderFillRect(renderer, &background_rect);
        SDL_PumpEvents();
        // mouse_released = 0;
        keyboard_state = SDL_GetKeyboardState(NULL);
        if (keyboard_state[SDL_SCANCODE_RETURN]) {
            returned = 1;
        }

        int key_pressed = 0;
        if (keyboard_state[SDL_SCANCODE_UP]) {
            key_pressed = 1;
            key_state = SDL_SCANCODE_UP;
        }
        if (keyboard_state[SDL_SCANCODE_DOWN]) {
            key_pressed = 1;
            key_state = SDL_SCANCODE_DOWN;
        }
        if (key_pressed == 0) {
            if (key_state == SDL_SCANCODE_UP) {
                n = (n - 1 < 0)? total - 1: n - 1;
            } else if (key_state == SDL_SCANCODE_DOWN) {
                n = (n + 1) % total;
            }
            key_state = 0;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < size - 1; i++) {
            double scale = 100.0*(double)size/201;
            SDL_RenderDrawLine(renderer, (int)(window_width*i/size), 
                                            window_height/2 - (int)(scale*eigenvectors(i, n).real()), 
                                   (int)(window_width*(i + 1)/size), 
                                   window_height/2 - (int)(scale*eigenvectors(i+1, n).real()));
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
        SDL_Delay(delay_time);
    } while(returned == 0);



    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
} 