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


int countOfLines(int n, int rank, int size) {
    return n / size + ((rank < n && n % size != 0) ? 1 : 0);
}

int firstLine(int n, int rank, int size) {
    int res = 0;
    for (int i = 0; i < rank; ++i) {
        res += countOfLines(n, i, size);
    }

    return res;
}

void initLinesSettings(int *linesCount, int *firstLines, int size, int n) {
    firstLines[0] = 0;
    linesCount[0] = countOfLines(n, 0, size);

    for (int i = 1; i < size; ++i) {
        linesCount[i] = countOfLines(n, i, size);
        firstLines[i] = firstLines[i - 1] + linesCount[i - 1];
    }
}
