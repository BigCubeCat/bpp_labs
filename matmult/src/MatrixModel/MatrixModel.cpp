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
