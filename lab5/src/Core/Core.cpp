#include <iostream>
#include "Core.h"

int Core::calculate(int num) {
    int result = 1;
    for (int a = 2; a <= num; ++a) {
        result *= a;
    }
    return result;
}