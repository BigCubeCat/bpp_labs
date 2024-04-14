#ifndef MATMULT_TCONFIGSTRUCT_H
#define MATMULT_TCONFIGSTRUCT_H

#include "../MatrixModel/MatrixModel.h"

struct TConfigStruct {
    int n = 0;
    int k = 0;
    int m = 0;
    MatrixModel *matrixA = nullptr;
    MatrixModel *matrixB = nullptr;

    explicit TConfigStruct();
    explicit TConfigStruct(int n, int m, int k);

    void print();
};

#endif
