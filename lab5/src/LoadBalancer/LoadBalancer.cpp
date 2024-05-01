#include "LoadBalancer.h"


LoadBalancer::LoadBalancer(int r, int count, int fc) :
        rank(r), countProcess(count), deltaCount(fc) {
    workload = new int[countProcess];
    reassignments = new int[countProcess];
}

LoadBalancer::~LoadBalancer() {
    delete[] workload;
    delete[] reassignments;
}

void LoadBalancer::updateCurrentCount(int countTasks) {
    workload[rank] = countTasks;
}

LoadBalancer::LoadBalancer() = default;

int *LoadBalancer::currentWorkload() const {
    return workload + rank;
}

bool LoadBalancer::hasAnyTasks() const {
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
