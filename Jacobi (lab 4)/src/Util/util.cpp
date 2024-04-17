#include <cmath>
#include "util.h"

double vectorSize(double x, double y, double z) {
    return x * x + y * y + z * z;
}

int cellCoord(int x, int x0, double h) {
    return static_cast<int>(std::round(x0 + x * h));
}

double maximumDouble(const double &a, const double &b)  {
    return (a > b) ? a : b;
}
