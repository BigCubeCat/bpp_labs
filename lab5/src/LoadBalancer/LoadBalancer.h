#ifndef LAB5_LOADBALANCER_H
#define LAB5_LOADBALANCER_H

#include <string>

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
public:
    int *workload{};
    int *reassignments{};
    LoadBalancer();

    LoadBalancer(int rank, int count, int fc);

    void updateCurrentCount(int countTasks);

    int *currentWorkload() const;

    bool hasAnyTasks() const;

    ~LoadBalancer();

    std::string toString();
};


#endif
