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
    bool imDone = false;

    pthread_attr_t pthreadAttr;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[2];

    TaskList taskList{};
    Core core{};
    Storage store;
    LoadBalancer loadBalancer{};

    bool useProfile;
    bool useBalance;

    int delay;
    bool debug;

    double timeSpent = 0;
    double disbalance = 0;
public:
    Worker(const Config &conf);

    ~Worker();

    bool noTasks();

    static void *calculator(void *ptr);
    static void *communicator(void *ptr);

    void DoOneTask();

    void Run();

    std::string getResult();
};


#endif
