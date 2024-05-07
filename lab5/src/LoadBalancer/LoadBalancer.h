#ifndef LAB5_LOADBALANCER_H
#define LAB5_LOADBALANCER_H

#include <string>
#include <vector>
#include "RankData.h"

/*
 * LoadBalancer
 * Содержит информацию о состоянии каждого из процессов (workload)
 * И информацию о требуемуих перегруппировках
 */
class LoadBalancer {
private:
    int rank{};
    int countProcess{};
    // Число задач, при которых нужно делать балансировку
    int criticalDiff{};
    std::vector<RankData> tmp;
    int maxDiff = 0;
public:
    int *workload = nullptr;
    int *reassignments = nullptr;
    int *counts = nullptr;

    LoadBalancer(int rank, int count, int fc);

    void updateCurrentCount(int countTasks);

    void balance();

    int *currentWorkload() const;

    bool hasAnyTasks() const;

    bool needToBalance() ;

    ~LoadBalancer();

    std::string toString() const;
};


#endif
