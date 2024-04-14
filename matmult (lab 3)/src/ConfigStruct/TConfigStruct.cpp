#include <iostream>
#include "./TConfigStruct.h"

TConfigStruct::TConfigStruct(int n, int m, int k) : n(n), k(k), m(m) {
    matrixA = new MatrixModel(n, m);
    matrixB = new MatrixModel(m, k);
}

void TConfigStruct::print() {
    std::cout << "A:\n";
    matrixA->printMatrix();
    std::cout << "B:\n";
    matrixB->printMatrix();
}

TConfigStruct::TConfigStruct() = default;
