#ifndef LAB5_LOADBALANCER_H
#define LAB5_LOADBALANCER_H


#include "TaskList.h"

/*
 * LoadBalancer
 * Содержит информацию о состоянии каждого из процессов,
 */
class LoadBalancer {
private:
    int totalTasks;
    std::vector<int> workload;
public:
    LoadBalancer(TaskList tl);
};


#endif
