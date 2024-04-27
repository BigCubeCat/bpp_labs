#include "ConfReader.h"

ConfReader::ConfReader(bool useEnv) {
    if (useEnv) {
        initFromEnv();
    } else {
        x0 = -1;
        y0 = -1;
        z0 = -1;
        Dx = 2;
        Dy = 2;
        Dz = 2;
        Nx = 800;
        Ny = 800;
        Nz = 800;
        a = 100000;
        epsilon = 0.00000001;
    }
    calculateH();
}


double ConfReader::parseEnvDouble(const char *variable) {
    return std::atof(getenv(variable));
}

int ConfReader::parseEnvInt(const char *variable) {
    return std::atoi(getenv(variable));
}

void ConfReader::calculateH() {
    hx = Dx / (Nx - 1);
    hy = Dy / (Ny - 1);
    hz = Dz / (Nz - 1);
}

void ConfReader::initFromEnv() {
    x0 = parseEnvDouble("x0");
    y0 = parseEnvDouble("y0");
    z0 = parseEnvDouble("z0");

    Dx = parseEnvDouble("Dx");
    Dy = parseEnvDouble("Dy");
    Dz = parseEnvDouble("Dz");

    Nx = parseEnvInt("Nx");
    Ny = parseEnvInt("Ny");
    Nz = parseEnvInt("Nz");

    a = parseEnvDouble("a");

    epsilon = parseEnvDouble("epsilon");
}
