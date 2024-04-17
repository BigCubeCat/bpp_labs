#include <iostream>
#include <cmath>
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

double Algo::getValue(int x, int y, int z, int fz) {
    if (inBounds(x, y, x)) {
        return p[linearIndex(x, y, z)];
    }
    return calcOutOfBounds(x, y, z, fz);
}

bool Algo::inBounds(int x, int y, int z) const {
    auto inBound = [](int value, int bound) -> bool { return (0 <= value && value < bound); };
    return inBound(x, config.Nx) && inBound(y, config.Ny) && inBound(z, config.Nz);
}

double Algo::calcOutOfBounds(int x, int y, int z, int fz) {
    // TODO переписать без вызова коструктора Vector3
    return Vector3(config.x0 + x * config.hx, config.y0 + y * config.hy, config.z0 + (z - 1 + fz) * config.hz).size();
}

Algo::~Algo() {
    delete[] p;
    delete[] pTemp;
}

double Algo::calcEpsilon() {
    double res = 0;
    int start = linearIndex(0, 0, 1);
    int finish = linearIndex(0, 0, config.a + 1);
    double value;
    for (int i = start; i < finish; ++i) {
        value = fabs(p[i] - pTemp[i]);
        res = (res < value) ? res : value;
    }
    return res;

}

void Algo::calcNextPhi(int a, int b, int fz) {
    for (int z = a; z < b; ++z) {
        for (int y = 0; y < config.Ny; ++y) {
            for (int x = 0; x < config.Nx; ++x) {
                calculate(x, y, z, fz);
            }
        }
    }
    swap();
}

void Algo::calculate(int x, int y, int z, int fz) {
    pTemp[linearIndex(x, y, z)] = (
            ((getValue(x + 1, y, z, fz) + getValue(x - 1, y, z, fz)) / hSquare.x
             +
             (getValue(x, y + 1, z, fz) + getValue(x, y - 1, z, fz)) / hSquare.y
             +
             (getValue(x, y, z + 1, fz) + getValue(x, y, z - 1, fz)) / hSquare.z
             -
             func(
                     config.x0 + x * config.hx, config.y0 + y * config.hy,
                     config.z0 + (z - 1 + fz) * config.hz,
                     config.a
             )));
}

bool Algo::isRunning() const {
    return !needExit;
}

void Algo::swap() {
    std::swap(p, pTemp);
}

double Algo::getMaxDelta() {
    double maximum = 0;
    double diff;
    for (int i = 0; i < config.Nx; ++i) {
        for (int j = 0; j < config.Ny; ++j) {
            for (int k = 0; k < config.Nz; ++k) {
                diff = fabs(getValue(i, j, k, 0) - func(i, j, k, config.a));
                maximum = (diff > maximum) ? diff : maximum;
            }
        }
    }
    return maximum;
}
