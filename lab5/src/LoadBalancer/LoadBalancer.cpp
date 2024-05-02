#include <algorithm>
#include <iostream>
#include "LoadBalancer.h"


LoadBalancer::LoadBalancer(int r, int count, int fc) :
        rank(r), countProcess(count), deltaCount(fc) {
    workload = new int[countProcess];
    reassignments = new int[countProcess];
    tmp.resize(countProcess);
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

void LoadBalancer::balance() {
    if (rank != 0) {
        return;
    }
    for (int i = 0; i < countProcess; ++i) {
        tmp[i].rank = i;
        tmp[i].workload = workload[i];
        reassignments[i] = -1;
    }
    std::sort(tmp.begin(), tmp.end());
    int difference;
    int first, second;
    for (first = 0; first < countProcess / 2; ++first) {
        second = countProcess - first - 1;
        difference = tmp[second].workload - tmp[first].workload;
        if (difference >= 2 * deltaCount) {
            reassignments[tmp[first].rank] = tmp[first].rank;
            reassignments[tmp[second].rank] = tmp[first].rank;
        }
    }
}
