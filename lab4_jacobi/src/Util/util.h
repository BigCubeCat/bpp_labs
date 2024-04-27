#ifndef JACOBI_UTIL_H
#define JACOBI_UTIL_H

#include "../Conf/ConfReader.h"

double vectorSize(double x, double y, double z);

double maximumDouble(const double &a, const double &b);

int countOfLines(int n, int rank, int size);

int firstLine(int n, int rank, int size);

int inBounds(int x, int y, int z, int sx, int sy, int sz);

double calculateDelta(const ConfReader &conf, double *res);

#endif
