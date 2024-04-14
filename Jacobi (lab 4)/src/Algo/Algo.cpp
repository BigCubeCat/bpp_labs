#include <iostream>
#include "Algo.h"

Algo::Algo(const ConfReader &conf, double (*f)(double, double, double, double)) : config(conf), func(f) {
    pSize = (config.Nx * config.Ny * (config.Nz + 2));
    p = new double[pSize];
    pTemp = new double[pSize];

    initNode(0, 0);

    hSquare = Vector3(
            1 / config.hx / config.hx,
            1 / config.hy / config.hy,
            1 / config.hz / config.hz
    );
    denumerator = conf.a + 2 * hSquare.x + 2 * hSquare.y + 2 * hSquare.z;
    std::cout << func(1, 1, 1, 1) << std::endl;
}

Vector3 Algo::nodeCoord(Vector3 point) {
    return Vector3(
            config.x0 + point.x * config.hx,
            config.y0 + point.y * config.hy,
            config.z0 + point.z * config.hz
    );
}

double Algo::calcNextPhi() {
    double result = 0;
    return result;
}

/*
 * Returns index in 1D array;
 * It's 3D row-major order ;-)
 */
int Algo::linearIndex(int x, int y, int z) {
    return z * config.Nx * config.Ny + y * config.Nz + z;
}

void Algo::initNode(int z, int fz) {
    int index;
    for (int y = 0; y < config.Ny; ++y) {
        for (int x = 0; x < config.Nx; ++x) {
            index = linearIndex(x, y, z);
            pTemp[index] = nodeCoord(Vector3(x, y, fz + z - 1)).size();
            p[index] = pTemp[index];
        }
    }
}

Algo::~Algo() {
    delete[] p;
    delete[] pTemp;
}
