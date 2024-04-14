#ifndef JACOBI_VECTOR3_H
#define JACOBI_VECTOR3_H


struct Vector3 {
    double x, y, z;
    explicit Vector3(double x, double y, double z);
    double size();
};


#endif
