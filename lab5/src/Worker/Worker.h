#ifndef LAB5_WORKER_H
#define LAB5_WORKER_H

#include <pthread.h>
#include <chrono>
#include <thread>

#include "Core.h"
#include "Config.h"
#include "Profiler.h"
#include "MutualMem.h"
#include "Logger.h"


class Worker {
private:
    int mpiRank, mpiSize;

    Config config;
    Core core;
    Profiler profiler;
    MutualMem *mem;

    Logger logger;

    pthread_attr_t commThreadAttr{};
    pthread_attr_t blncThreadAttr{};
    pthread_attr_t workThreadAttr{};
    pthread_t threads[3]{};

    bool debug;

    int *swapBuff = nullptr;

    void getTiming();

    void fetchTasks(int rank, int count);

    void giveTask(int rank);

    // функции потоков
    void workerThread();

    void communicatorThread();

    void balancerThread();

    // точки входа в потоки
    static void *workerThreadFunction(void *ptr);

    static void *communicatorThreadFunction(void *ptr);

    static void *balancerThreadFunction(void *ptr);

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
