#ifndef JACOBI_CONFREADER_H
#define JACOBI_CONFREADER_H


#include <string>

class ConfReader {
public:
    ConfReader(bool useEnv);

    double x0 = 0, y0 = 0, z0 = 0;
    double Dx = 0, Dy = 0, Dz = 0;
    int Nx = 0, Ny = 0, Nz = 0;
    double hx{}, hy{}, hz{};
    double epsilon = 0;
    double a = 0.001;

    bool invalid = false;
private:
    void initFromEnv();
    static double parseEnvDouble(const char *variable);

    static int parseEnvInt(const char *variable);

    void calculateH();
};


#endif
