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

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[2];

    TaskList taskList;
    Core core{};
    Storage store;
    EStatus status;

    unsigned long long result;
public:
    Worker(int storeSize);

    ~Worker();

    bool noTasks();

    static void *receiver(void *ptr);

    void DoOneTask();

    void Run(int request);

    unsigned long long getResult();
};


#endif
