#include <complex>
#include <Eigen/Core>
#include <vector>
#include "complex_float_rgba.hpp"

#pragma once

using namespace Eigen;

void double_matrix_to_rgba_array(ComplexFloatRGBA *arr, 
                                 const MatrixXd &matrix,
                                 size_t width, size_t height);


void complex_double_matrix_to_rgba_array(ComplexFloatRGBA *arr, 
                                         const MatrixXcd &matrix,
                                         size_t width, size_t height);


void vector_slice_to_rgba_array(ComplexFloatRGBA *arr, double amp,
                                const MatrixXcd &vector_matrix, 
                                size_t which_column,
                                size_t width, size_t height);


void double_matrix_to_rgba_array(std::vector<ComplexFloatRGBA> &arr,
                                 MatrixXd &matrix,
                                 size_t width, size_t height);


void complex_double_matrix_to_rgba_array(std::vector<ComplexFloatRGBA> &arr,
                                         MatrixXcd &matrix,
                                         size_t width, size_t height);


void vector_slice_to_rgba_array(std::vector<ComplexFloatRGBA> &arr, double amp,
                                MatrixXcd &vector_matrix, size_t which_column,
                                size_t width, size_t height);

