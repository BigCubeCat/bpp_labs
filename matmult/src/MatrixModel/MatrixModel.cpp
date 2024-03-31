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

MatrixModel operator*(const MatrixModel &a, const MatrixModel &b) {
    size_t rows = a.height();
    size_t cols = b.width();
    size_t stripSize = a.width();
    MatrixModel result = MatrixModel(rows, b.height());
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < b.height(); ++col) {
            result.setValue(row, col, 0);
            for (size_t i = 0; i < stripSize; ++i) {
                result.data[row * cols + col] += a.getValue(row, i) * b.getValue(col, i);
            }
        }
    }
    return result;
}

