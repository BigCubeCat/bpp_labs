#include "ConfReader.h"

ConfReader::ConfReader() {
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
