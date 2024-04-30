#ifndef JACOBI_ALGO_H
#define JACOBI_ALGO_H


#include "../Conf/ConfReader.h"
#include "Vector3.h"

class Algo {
public:
    double *data;
    double *tempData;

    Algo(
            const ConfReader &conf,
            double (*f)(double, double, double, double),
            int cntz,
            int fz,
            int cntEl
    );

    ~Algo();

    double *getDataPointer(int z);

    void calculate(int a, int b);

    void swapArrays();

    double getEpslion(const int cntz);

private:
    ConfReader config;
    int countElements;
    int countZ;
    int firstZ;

    double (*func)(double, double, double, double);

    bool stopped = false;
    Vector3 hSquare = Vector3(0, 0, 0);
    double denumerator = 1;

    /*
     * initEdge()
     * calculate values on deges
     */
    void initEdge() const;

    /*
     * double getValue(int x, int y, int z);
     * return function value in cell
     */
    double getValue(int x, int y, int z);

    /*
     * Returns index in 1D array;
     * It's 3D row-major order ;-)
     */
    int layerIndex(int x, int y, int z) const;

    /*
     * calcNumerator(Vector3 vec, int x, int y, int z)
     * calculate numerator in cell
     */
    double calcNumerator(Vector3 vec, int i, int j, int k);

};


#endif
