#ifndef JACOBI_ALGO_H
#define JACOBI_ALGO_H


#include "../Conf/ConfReader.h"
#include "Vector3.h"

class Algo {
public:
    int mpiRank;
    int dataSize;
    int extendSize;
    double *data;
    double *tempData;
    double maximumDifference = 0; // максимальное отклонение от *искомой* функции
    int layerSize;

    Algo(const ConfReader &conf, double (*f)(double, double, double, double), int rank);

    ~Algo();

    bool isStopped() const;

    void calcNextPhi(int layerPosition, int layerNumber);

    double getMaxDelta();

    void swapArrays();

    void checkStopped();

private:
    ConfReader config;

    double (*func)(double, double, double, double);

    bool stopped = false;
    Vector3 hSquare = Vector3(0, 0, 0); // вектор с квадратами h
    double denumerator = 1; // Знаменатель в формуле (он константен по сути)

    /*
     * double getValue(int x, int y, int z);
     * return function value in cell
     */
    double getValue(int x, int y, int z);

    /*
     * double calcDiff()
     * find absolute maximum difference btw
     * current result and original function result
     */
    double calcDiff();

    /*
     * Returns index in 1D array;
     * It's 3D row-major order ;-)
     */
    int layerIndex(int x, int y, int z) const;

    double calcNumerator(Vector3 vec, int i, int j, int k);


};


#endif
