#ifndef SOLVE_H
#define SOLVE_H

#include "../CtxData/CtxData.h"

double calc(CtxData *data, double tao, int rank);

double sumVector(const double *vector, int size);

int solve(CtxData *data, double tao, int rank, size_t max_iterations);

#endif
