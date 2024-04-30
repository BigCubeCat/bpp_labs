#include <cmath>
#include <cfloat>
#include "util.h"


int countOfLines(int n, int rank, int size) {
    return n / size + (rank < n % size ? 1 : 0);
}

int firstLine(int n, int rank, int size) {
    int res = 0;
    for (int i = 0; i < rank; ++i) {
        res += countOfLines(n, i, size);
    }
    return res;
}

int inBounds(int x, int y, int z, int sx, int sy, int sz) {
    return 0 <= x && x < sx && 0 <= y && y < sy && 0 <= z && z < sz;
}

/*
 * по сути фи из условия. Надо бы тоже вынести в лямбду
 */
double vectorSize(double x, double y, double z) {
    return x * x + y * y + z * z;
}

int cellCoord(int x, int x0, double h) {
    return static_cast<int>(std::round(x0 + x * h));
}

double maximumDouble(const double &a, const double &b) {
    return (a > b) ? a : b;
}

double calculateDelta(const ConfReader &conf, double *res) {
    auto deltaMax = DBL_MAX;
    double x, y, z;
    double tmp;
    auto cellCoord = [](int i, int c0, double h) -> int {
        return static_cast<int>(c0 + i * h);
    };
    auto layerPosition = [](int x, int y, int z, int Nx, int Ny, int Nz) -> int {
        return z * Nx * Ny + y * Nx + x;
    };
    for (int i = 0; i < conf.Nx; i++) {
        x = cellCoord(i, conf.x0, conf.hx);
        for (int j = 0; j < conf.Ny; j++) {
            y = cellCoord(j, conf.y0, conf.hy);
            for (int k = 0; k < conf.Nz; k++) {
                z = cellCoord(k, conf.z0, conf.hz);
                tmp = fabs(
                        res[
                                layerPosition(i, j, k, conf.Nx, conf.Ny, conf.Nz)
                        ] - vectorSize(x, y, z)
                );
                deltaMax = maximumDouble(deltaMax, tmp);
            }
        }
    }
    return deltaMax;
}
