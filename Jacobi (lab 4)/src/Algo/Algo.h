#ifndef JACOBI_ALGO_H
#define JACOBI_ALGO_H


#include "../Conf/ConfReader.h"
#include "Vector3.h"

class Algo {
private:
    ConfReader config;

    double (*func)(double, double, double, double);

    Vector3 hSquare = Vector3(0, 0, 0);
    double denumerator = 0;

    double *p;
    double *pTemp;
    int pSize;

    Vector3 nodeCoord(Vector3 ijk);

    void initNode(int z, int fz);


public:
    explicit Algo(const ConfReader &conf, double (*func)(double, double, double, double));

    ~Algo();

    double calcNextPhi();

    int linearIndex(int x, int y, int z);

};


#endif
