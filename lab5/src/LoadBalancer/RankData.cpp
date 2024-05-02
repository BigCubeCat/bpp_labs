#include "RankData.h"

bool RankData::operator<(const RankData &other) {
    if (workload < other.workload) return true;
    return (rank < other.rank);
}
