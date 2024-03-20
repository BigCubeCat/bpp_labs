#ifndef MATRIX_MODEL_H
#define MATRIX_MODEL_H

#include <cstdio>
#include <cstdlib>

struct MatrixModel {
    double *data{};
    size_t width = 0;
    size_t height = 0;

    explicit MatrixModel(size_t rows, size_t cols);

    void setValue(size_t row, size_t col, double value) const;

    double getValue(size_t row, size_t col);

    void printMatrix();

    void Malloc();

    ~MatrixModel();
};


#endif
