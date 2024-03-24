#include <iostream>
#include "./MatrixModel.h"

MatrixModel::MatrixModel(size_t rows, size_t cols) : width(cols), height(rows) {
    Malloc();
}

void MatrixModel::Malloc() {
    data = new double[width * height];
}

MatrixModel::~MatrixModel() {
    delete[] data;
}

void MatrixModel::setValue(size_t row, size_t col, double value) const {
    data[row * width + col] = value;
}

void MatrixModel::printMatrix() {
    for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
            std::cout << getValue(row, col) << " ";
        }
        std::cout << std::endl;
    }
}

double MatrixModel::getValue(size_t row, size_t col) {
    return data[row * width + col];
}
