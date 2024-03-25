#include "./algo.h"
#include <omp.h>


void multMatrixPart(const double *A, const double *B, double *C, size_t stripSize, size_t rows, size_t cols) {
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            C[row * cols + col] = 0;
#pragma omp parallel for
            for (size_t i = 0; i < stripSize; ++i) {
                C[row * cols + col] += A[row * cols + i] * B[i * cols + col];
            }
        }
    }
}