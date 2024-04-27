#include "Vector3.h"
#include "../Util/util.h"

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {

}

double Vector3::size() const {
    return vectorSize(x, y, z);
}

bool Vector3::operator==(const Vector3 &other) const {
    return (x == other.x) && (y == other.y) && (z == other.z);
}
