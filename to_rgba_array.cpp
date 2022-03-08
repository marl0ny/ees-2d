#include "to_rgba_array.hpp"


void double_matrix_to_rgba_array(ComplexFloatRGBA *arr, 
                                 const MatrixXd &matrix,
                                 size_t width, size_t height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            arr[(int)width*i + j].r = (float)matrix(i, j); 
            arr[(int)width*i + j].g = 0.0;
            arr[(int)width*i + j].g = 0.0;
            arr[(int)width*i + j].a = 1.0;
        }
    }
}

void complex_double_matrix_to_rgba_array(ComplexFloatRGBA *arr, 
                                         const MatrixXcd &matrix,
                                         size_t width, size_t height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            arr[(int)width*i + j].re = (float)matrix(i, j).real();
            arr[(int)width*i + j].im = (float)matrix(i, j).imag(); 
            arr[(int)width*i + j].a = 1.0;
        }
    }
}

void vector_slice_to_rgba_array(ComplexFloatRGBA *arr, double amp,
                                const MatrixXcd &vector_matrix, 
                                size_t which_column,
                                size_t width, size_t height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double re = amp*vector_matrix(i*width + j, which_column).real();
            double im = amp*vector_matrix(i*width + j, which_column).imag();
            arr[((int)width)*i + j].real((float)re);
            arr[((int)width)*i + j].imag((float)im);
            arr[((int)width)*i + j].a = 1.0;
        }
    }
}

