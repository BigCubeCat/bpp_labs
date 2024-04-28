#include "Core.h"

unsigned long long factorial(unsigned long long num) {
    unsigned long long result = 1;
    for (unsigned long long a = 1; a <= num; ++a) {
        result *= a;
    }
    return result;
}