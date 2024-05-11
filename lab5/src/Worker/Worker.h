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

    EProcessStatus *processWorkload;

    int smolTimeout = 0;
    int *swapBuff = nullptr;

    void getTiming();

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
            Core core,
            MutualMem *m,
            const Config &conf
    );

    ~Worker();

    void Run();

    std::string getResult();

    static int countTasksInProcess(int n, int rank, int size);

    static int firstLine(int n, int rank, int size);

};


#endif
