#include "pi.h"

#include <cstdlib>

struct TPoint {
    double_t x, y;
};

double_t randint() { return ((double_t)rand()) / (double_t)(RAND_MAX); }

TPoint generatePoint() {
    TPoint result;
    result.x = randint();
    result.y = randint();
    return result;
}

int inCircle(TPoint p) { return ((p.x * p.x) + (p.y * p.y)) <= 1; }

double_t calcPi(int count) {
    size_t countIn = 0;
    for (size_t i = 0; i < count; ++i) {
        countIn += inCircle(generatePoint());
    }
    return 4 * ((double_t)countIn / (double_t)count);
}
