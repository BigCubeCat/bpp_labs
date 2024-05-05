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
    // Число задач, которое мы предаем между двумя процессами за раз
    int deltaCount{};
    std::vector<RankData> tmp;
public:
    int *workload = nullptr;
    int *reassignments = nullptr;

    LoadBalancer(int rank, int count, int fc);

    void updateCurrentCount(int countTasks);

    void balance();

    int *currentWorkload() const;

    bool hasAnyTasks() const;

    ~LoadBalancer();

    std::string toString();
};


#endif
