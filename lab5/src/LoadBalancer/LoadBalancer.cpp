#include <algorithm>
#include <iostream>
#include "LoadBalancer.h"


LoadBalancer::LoadBalancer(int r, int count, int c) :
        rank(r), countProcess(count), criticalDiff(c) {
    workload = new int[countProcess];
    reassignments = new int[countProcess];
    counts = new int[countProcess];
    tmp.resize(countProcess);
}

LoadBalancer::~LoadBalancer() {
    delete[] workload;
    delete[] reassignments;
}

void LoadBalancer::updateCurrentCount(int countTasks) {
    workload[rank] = countTasks;
}

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

std::string LoadBalancer::toString() const {
    std::string res = "-----\nWorkload:\n";
    for (int i = 0; i < countProcess; ++i) {
        res += std::to_string(workload[i]) + " ";
    }
    res += "\n-----\n";
    return res;
}

void LoadBalancer::balance() {
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
        if (difference >= criticalDiff) {
            reassignments[tmp[first].rank] = tmp[first].rank;
            reassignments[tmp[second].rank] = tmp[first].rank;
            counts[tmp[first].rank] = counts[tmp[second].rank] = difference / 2;
        }
    }
}

bool LoadBalancer::needToBalance() {
    int maximum = workload[0], minimum = workload[0];
    for (int i = 1; i < countProcess; ++i) {
        if (workload[i] > maximum) {
            maximum = workload[i];
        } else if (workload[i] < minimum) {
            minimum = workload[i];
        }
    }
    maxDiff = maximum - minimum;
    std::cout << "maxDiff = " << maxDiff << std::endl;
    return maxDiff >= criticalDiff;
}
