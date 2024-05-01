#include "LoadBalancer.h"


LoadBalancer::LoadBalancer(int r, int count, int fc) : rank(r), countProcess(count), freeCount(fc) {
    workload = new int[countProcess];
}

LoadBalancer::~LoadBalancer() {
    delete[] workload;
}

void LoadBalancer::updateCurrentCount(int countTasks) {
    workload[rank] = countTasks;
}

bool LoadBalancer::isFree() {
    return workload[rank] < freeCount;
}

LoadBalancer::LoadBalancer() = default;

int *LoadBalancer::currentWorkload() const {
    return workload + rank;
}

bool LoadBalancer::hasAnyTasks() {
    int sum = 0;
    for (int i = 0; i < countProcess; ++i) {
        sum += workload[i];
    }
    return sum > 0;
}

std::string LoadBalancer::toString() {
    std::string res = "Workload:\n";
    for (int i = 0; i < countProcess; ++i) {
        res += std::to_string(workload[i]) + " ";
    }
    res += "\n-----\n";
    return res;
}
