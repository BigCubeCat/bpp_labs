#include "Vector3.h"

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {

}

double Vector3::size() {
    return x * x + y * y + z * z;
}
