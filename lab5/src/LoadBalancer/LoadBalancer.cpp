#include "LoadBalancer.h"


LoadBalancer::LoadBalancer(int r, int count) : rank(r), countProcess(count) {
    workload = new int[countProcess];
}

LoadBalancer::~LoadBalancer() {
    delete[] workload;
}

void LoadBalancer::updateCurrentCount(int countTasks) {
    workload[rank] = countTasks;
}

bool LoadBalancer::isFree() {
    return workload[rank] < 2;
}

LoadBalancer::LoadBalancer() {

}

int *LoadBalancer::currentWorkload() const {
    return workload + rank;
}
