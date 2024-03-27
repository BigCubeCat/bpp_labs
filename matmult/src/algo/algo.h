#ifndef MATMULT_ALGO_H
#define MATMULT_ALGO_H

#include <cstdio>

void multMatrixPart(const double *A, const double *B, double *C, size_t stripSize, size_t rows, size_t cols);

int countOfLines(int n, int rank, int size);

int firstLine(int n, int rank, int size);

void initLinesSettings(int *linesCount, int *firstLines, int size, int n);


#endif
