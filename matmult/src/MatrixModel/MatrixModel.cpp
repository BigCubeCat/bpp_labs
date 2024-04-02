#include <omp.h>
#include <iostream>

#include "./MatrixModel.h"

MatrixModel::MatrixModel(size_t rows, size_t cols) : countRows(rows), countColumns(cols) {
    dataSize = countColumns * countRows;
    data = new double[dataSize];
}

MatrixModel::~MatrixModel() {
    delete[] data;
}

void MatrixModel::setValue(size_t row, size_t col, double value) const {
    data[row * width() + col] = value;
}

void MatrixModel::printMatrix() const {
    std::cout << "Matrix with " << countRows << " rows and " << countColumns << " columns\n";
    for (size_t row = 0; row < countRows; ++row) {
        for (size_t col = 0; col < countColumns; ++col) {
            std::cout << getValue(row, col) << " ";
        }
        std::cout << std::endl;
    }
}

double MatrixModel::getValue(size_t row, size_t col) const {
    return data[row * width() + col];
}

size_t MatrixModel::width() const {
    return countColumns;
}

size_t MatrixModel::height() const {
    return countRows;
}

void MatrixModel::copy(const MatrixModel &other) const {
    for (int i = 0; i < other.height(); ++i) {
        for (int j = 0; j < other.width(); ++j) {
            setValue(i, j, other.getValue(i, j));
        }
    }
}

MatrixModel operator*(const MatrixModel &a, const MatrixModel &b) {
    size_t rows = a.height();
    size_t cols = b.height();
    size_t stripSize = b.width();
    MatrixModel result = MatrixModel(rows, cols);
#pragma omp parallel for
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.setValue(row, col, 0);
            for (size_t k = 0; k < stripSize; ++k) {
                result.data[row * cols + col] += a.getValue(row, k) * b.getValue(col, k);
            }
        }
    }
    return result;
}

