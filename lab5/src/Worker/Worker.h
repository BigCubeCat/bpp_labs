#ifndef LAB5_WORKER_H
#define LAB5_WORKER_H

#include <pthread.h>

#include "Storage.h"
#include "EStatus.h"
#include "Core.h"
#include "TaskList.h"

class Worker {
private:
    int mpiRank, mpiSize;
    bool isMasterProcess;

    pthread_attr_t pthreadAttr;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[2];

    TaskList taskList{};
    Core core{};
    Storage store;
    LoadBalancer *loadBalancer;

public:
    Worker(int storeSize);

    ~Worker();

    bool noTasks();

    static void *calculator(void *ptr);
    static void *receiver(void *ptr);

    void DoOneTask();

    void Run();

    Storage getResult();
};


#endif
