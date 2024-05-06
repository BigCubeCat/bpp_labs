#ifndef LAB5_WORKER_H
#define LAB5_WORKER_H

#include <pthread.h>
#include <chrono>
#include <thread>

#include "Storage.h"
#include "EStatus.h"
#include "Core.h"
#include "TaskList.h"
#include "../LoadBalancer/LoadBalancer.h"
#include "Config.h"


class Worker {
private:
    int mpiRank, mpiSize;

    Config config;

    pthread_attr_t commThreadAttr;
    pthread_attr_t workThreadAttr;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[2];

    TaskList taskList;
    Storage store;
    LoadBalancer loadBalancer;

    bool useBalance;

    int delay;
    bool debug;

    double timeSpent = 0;
    double disbalance = -1;
    double maxTime = -1;
    double minTime = -1;

    int swapBuff;

    void readFromBuffer();

    void writeToBuffer();

public:
    Worker(int rank, int size, const Config &conf);

    ~Worker();

    bool noTasks();

    static void *workerThread(void *ptr);

    static void *communicatorThread(void *ptr);

    void DoOneTask();

    void Run();

    void doBalance();

    std::string getResult() const;
};


#endif
