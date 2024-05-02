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

    pthread_attr_t pthreadAttr;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[2];

    TaskList taskList{};
    Storage store;
    LoadBalancer loadBalancer{};

    bool useProfile;
    bool useBalance;

    int delay;
    bool debug;

    double timeSpent = -1;
    double disbalance = 0;

    int swapBuff;

    void readFromBuffer();

    void writeToBuffer();

public:
    Worker(const Config &conf);

    ~Worker();

    bool noTasks();

    static void *workerThread(void *ptr);

    static void *communicatorThread(void *ptr);

    void DoOneTask();

    void Run();

    void doBalance();

    std::string getResult();
};


#endif
