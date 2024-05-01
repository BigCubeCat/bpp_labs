#ifndef LAB5_LOADBALANCER_H
#define LAB5_LOADBALANCER_H

#include <string>

/*
 * LoadBalancer
 * Содержит информацию о состоянии каждого из процессов,
 */
class LoadBalancer {
private:
    int rank{};
    int countProcess{};
    int freeCount{};
public:
    int *workload{}; // массив с информацией от
    LoadBalancer();

    LoadBalancer(int rank, int count, int fc);

    void updateCurrentCount(int countTasks);

    int *currentWorkload() const;

    bool isFree();

    bool hasAnyTasks();

    ~LoadBalancer();

    std::string toString();
};


#endif
