#include <cmath>
#include <cstring>
#include "Algo.h"
#include "../Util/util.h"

Algo::Algo(
        const ConfReader &conf,
        double (*f)(double, double, double, double),
        int cntz,
        int fz,
        int cntEl
) : countElements(cntEl), countZ(cntz), firstZ(fz), config(conf), func(f) {

    // "сетка" с значениями
    data = new double[countElements];
    tempData = new double[countElements];
    memset(data, 0, countElements);

    // вектор с квадратами h. Считаем так как значение не меняется в дальнейшем
    hSquare = Vector3(config.hx * config.hx, config.hy * config.hy, config.hz * config.hz);

    // считаем знаменатель, так как его значение не меняется от итерации к итерации
    denumerator = config.a + 2 / hSquare.x + 2 / hSquare.y + 2 / hSquare.z;

    initEdge();
}

int Algo::layerIndex(int x, int y, int z) const {
    return z * config.Nx * config.Ny + y * config.Nx + x;
}

void Algo::calculate(int a, int b) {
    Vector3 vec = Vector3(0, 0, 0);
    for (int z = a; z < b; ++z) {
        vec.z = config.z0 + (z - 1 + firstZ) * config.hz;
        for (int y = 0; y < config.Ny; ++y) {
            vec.y = config.y0 + y * config.hy;
            for (int x = 0; x < config.Nx; ++x) {
                vec.x = config.x0 + x * config.hx;
                tempData[layerIndex(x, y, z)] = calcNumerator(vec, x, y, z) / denumerator;
            }
        }
    }
}

double Algo::calcNumerator(Vector3 vec, int i, int j, int k) {
    // просто реализация формулы)
    return (
            (getValue(i + 1, j, k) + getValue(i - 1, j, k)) / hSquare.x +
            (getValue(i, j + 1, k) + getValue(i, j - 1, k)) / hSquare.y +
            (getValue(i, j, k + 1) + getValue(i, j, k - 1)) / hSquare.z -
            func(vec.x, vec.y, vec.z, config.a)
    );
}

void Algo::swapArrays() {
    std::swap(data, tempData);
}

double Algo::getValue(int x, int y, int z) {
    if (inBounds(x, y, z, config.Nx, config.Ny, config.Nz)) {
        return data[layerIndex(x, y, z)];
    }
    return vectorSize(
            config.x0 + x * config.hx,
            config.y0 + y * config.hy,
            config.z0 + z * config.hz
    );
}

Algo::~Algo() {
    delete[] data;
    delete[] tempData;
}

void Algo::initEdge() const {
    for (int y = 0; y < config.Ny; ++y) {
        for (int x = 0; x < config.Nx; ++x) {
            data[layerIndex(x, y, 0)] = tempData[layerIndex(x, y, 0)] = vectorSize(
                    config.x0 + x * config.hx,
                    config.y0 + y * config.hy,
                    config.z0 + (firstZ - 1) * config.hz
            );
            data[layerIndex(x, y, countZ + 1)] = tempData[layerIndex(x, y, countZ + 1)] = vectorSize(
                    config.x0 + x * config.hx,
                    config.y0 + y * config.hy,
                    config.z0 + (firstZ + countZ) * config.hz
            );
        }
    }
}

double *Algo::getDataPointer(int z) {
    return data + layerIndex(0, 0, z);
}

double Algo::getEpslion(const int cntz) {
    double res = 0;
    int begin = layerIndex(0, 0, 1);
    int end = layerIndex(0, 0, cntz + 1);
    for (int i = begin; i < end; ++i) {
        res = maximumDouble(res, data[i] - tempData[i]);
    }
    return 0;
}
