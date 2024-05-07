#ifndef LAB5_WORKER_H
#define LAB5_WORKER_H

#include <pthread.h>
#include <chrono>
#include <thread>

#include "Core.h"
#include "Config.h"
#include "Profiler.h"
#include "MutualMem.h"


class Worker {
private:
    int mpiRank, mpiSize;

    Config config;
    Core core;
    Profiler profiler;
    MutualMem *mem;

    pthread_attr_t commThreadAttr{};
    pthread_attr_t blncThreadAttr{};
    pthread_attr_t workThreadAttr{};
    pthread_t threads[3]{};

    bool debug;

    int *swapBuff = nullptr;

    void getTiming();

    void askForTask();

    void giveTask(int rank);

    static void *workerThread(void *ptr);

    static void *communicatorThread(void *ptr);

    static void *balancerThread(void *ptr);

public:
    Worker(
            int rank, int size,
            MutualMem *m,
            const Config &conf
    );

    ~Worker();

    void Run();

    std::string getResult();

};


#endif
