#ifndef MATRIX_MODEL_H
#define MATRIX_MODEL_H

#include <cstdio>
#include <cstdlib>

struct MatrixModel {
    double *data;
    size_t dataSize = 0;
    size_t countRows = 0;
    size_t countColumns = 0;

    explicit MatrixModel(size_t rows, size_t cols);

    void setValue(size_t row, size_t col, double value) const;

    double getValue(size_t row, size_t col) const;

    void printMatrix() const;

    size_t width() const;
    size_t height() const;

    ~MatrixModel();
};

MatrixModel operator*(const MatrixModel& a, const MatrixModel& b);


#endif
