#ifndef JACOBI_ALGO_H
#define JACOBI_ALGO_H


#include "../Conf/ConfReader.h"
#include "Vector3.h"

class Algo {
private:
    ConfReader config;

    double (*func)(double, double, double, double);

    bool needExit = false;
    Vector3 hSquare = Vector3(0, 0, 0);
    double denumerator = 1;
    double *p;
    double *pTemp;
    int pSize;

    Vector3 nodeCoord(Vector3 ijk);

    void initNode(int z, int fz);

    double getValue(int x, int y, int z, int fz);

    bool inBounds(int x, int y, int z) const;

    double calcOutOfBounds(int x, int y, int z, int fz);

    void calculate(int x, int y, int z, int fz);

    double calcEpsilon();

    int linearIndex(int x, int y, int z);

    void swap();

public:
    Algo(const ConfReader &conf, double (*f)(double, double, double, double));

    ~Algo();

    bool isRunning() const;

    void calcNextPhi(int a, int b, int fz);

    double getMaxDelta();
};


#endif
