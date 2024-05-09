#include "pi.h"

#include <cstdlib>
#include <csignal>

struct TPoint {
    double_t x, y;
};

double_t randint() { return ((double_t) rand()) / (double_t)(RAND_MAX); }

TPoint generatePoint() {
    TPoint result;
    result.x = randint();
    result.y = randint();
    return result;
}

int inCircle(TPoint p) { return ((p.x * p.x) + (p.y * p.y)) <= 1; }

double_t calcPi(int count) {
    size_t countIn = 0;
    sleep(count);
    return 4 * ((double_t) countIn / (double_t) count);
}
