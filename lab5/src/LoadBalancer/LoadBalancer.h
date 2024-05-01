#ifndef LAB5_LOADBALANCER_H
#define LAB5_LOADBALANCER_H

/*
 * LoadBalancer
 * Содержит информацию о состоянии каждого из процессов,
 */
class LoadBalancer {
private:
    int rank;
    int countProcess;
public:
    int *workload; // массив с информацией от
    LoadBalancer();
    LoadBalancer(int rank, int count);

    void updateCurrentCount(int countTasks);

    int *currentWorkload() const;

    bool isFree();

    ~LoadBalancer();
};


#endif
