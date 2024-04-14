#include "./utils.h"

std::pair <int, int> getOptimalGeometry(int n) {
    std::pair<size_t, size_t> result;
    for (int i = 1; i * i < n; ++i) {
        if (n % i == 0) {
            result.first = n / i;
            result.second = i;
        }
    }
    return result;
}
