#include <cmath>
#include "Algo.h"
#include "../Util/util.h"

Algo::Algo(const ConfReader &conf, double (*f)(double, double, double, double), int rank) : config(conf), func(f),
                                                                                            mpiRank(rank) {
    layerSize = config.Nx * config.Ny;
    extendSize = config.Nz + 2;
    dataSize = (config.Nx * config.Ny * extendSize);
    data = new double[dataSize];
    tempData = new double[dataSize];

    hSquare = Vector3(config.hx * config.hx, config.hy * config.hy, config.hz * config.hz);
    // Nz = size

    denumerator = config.a + 2 / hSquare.x + 2 / hSquare.y + 2 / hSquare.z;

    Vector3 vector3 = Vector3(0, 0, 0);
    Vector3 zeroPoint = Vector3(config.x0, config.y0, config.z0);
    Vector3 diffVector = Vector3(config.x0 + config.Dx, config.y0 + config.Dy, config.z0 + config.Dz);
    for (int i = 0; i < config.Nx; ++i) {
        vector3.x = cellCoord(i, config.x0, config.hx);
        for (int j = 0; j < config.Ny; ++j) {
            vector3.y = cellCoord(j, config.y0, config.hy);
            for (int k = 0; k < extendSize; ++k) {
                vector3.z = cellCoord(i + (mpiRank * config.Nz - 1), config.z0, config.hz);
                if ((vector3 == zeroPoint) || (vector3 == diffVector)) {
                    data[layerIndex(i, j, k)] = vector3.size();
                } else {
                    data[layerIndex(i, j, k)] = 0;
                }
            }
        }
    }
}

double Algo::calcDiff() {
    double res, x, y, z, tmp;
    for (int i = 0; i < config.Nx; i++) {
        x = cellCoord(i, config.Nx, config.hx);
        for (int j = 0; j < config.Ny; j++) {
            y = cellCoord(j, config.y0, config.hy);
            for (int k = 0; k < config.Nz; k++) {
                z = cellCoord(k, config.z0, config.hz);
                tmp = fabs(data[layerIndex(i, j, k)] - vectorSize(x, y, z));
                res = res > tmp ? res : tmp;
            }
        }
    }
    return res;
}

int Algo::layerIndex(int x, int y, int z) const {
    // Возможно ошибка
    return z * config.Nx * config.Ny + y * config.Nz + x;
}


void Algo::calcNextPhi(int layerPosition, int layerNumber) {
    int absZ = layerNumber + layerPosition;
    auto onBorder = [](int value, int bound) -> bool { return (value == 0 || value == bound - 1); };
    maximumDifference = 0;

    Vector3 vec = Vector3(0, 0, cellCoord(absZ, config.z0, config.hz));
    int index;
    for (int j = 0; j < config.Nx; ++j) {
        vec.x = cellCoord(j, config.x0, config.hx);
        for (int k = 0; k < config.Ny; ++k) {
            vec.y = cellCoord(k, config.y0, config.hy);
            index = layerIndex(layerNumber, j, k);
            if (onBorder(j, config.Nx) || onBorder(k, config.Ny)) {
                tempData[index] = data[index];
            } else {
                tempData[index] = calcNumerator(vec, layerNumber, j, k) / denumerator;
                maximumDifference = maximumDouble(fabs(tempData[index] - data[index]), maximumDifference);
            }
        }
    }
}

double Algo::calcNumerator(Vector3 vec, int i, int j, int k) {
    return (
            (data[layerIndex(i + 1, j, k)] + data[layerIndex(i - 1, j, k)]) / hSquare.x +
            (data[layerIndex(i, j + 1, k)] + data[layerIndex(i, j - 1, k)]) / hSquare.y +
            (data[layerIndex(i, j, k + 1)] + data[layerIndex(i, j, k - 1)]) / hSquare.z +
            func(vec.x, vec.y, vec.z, config.a)
    );
}

bool Algo::isStopped() const {
    return stopped;
}

void Algo::swapArrays() {
    std::swap(data, tempData);
}

double Algo::getMaxDelta() {
    double maximum = 0;
    double diff;
    for (int i = 0; i < config.Nx; ++i) {
        for (int j = 0; j < config.Ny; ++j) {
            for (int k = 0; k < config.Nz; ++k) {
                diff = fabs(getValue(i, j, k) - func(i, j, k, config.a));
                maximum = maximumDouble(maximum, diff);
            }
        }
    }
    return maximum;
}

double Algo::getValue(int x, int y, int z) {
    return data[layerIndex(x, y, z)];
}

Algo::~Algo() {
    delete[] data;
    delete[] tempData;
}

void Algo::checkStopped() {
    stopped = maximumDifference <= config.epsilon;
}
