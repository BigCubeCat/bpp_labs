#include "Core.h"
#include "pi/pi.h"

std::string Core::calculate(int num) {
    return std::to_string(calcPi(num));
}
