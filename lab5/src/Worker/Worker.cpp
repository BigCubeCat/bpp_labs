#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

Worker::Worker(int storeSize) : store(Storage(storeSize)) {
    status = FREE;
    int threadProvided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &threadProvided);
    if (threadProvided != MPI_THREAD_MULTIPLE) { // если нельзя в многопоточность
        status = MPI_ERROR;
        return;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    pthread_mutex_init(&mutex, nullptr);
    pthread_attr_t pthreadAttr;

    pthread_attr_init(&pthreadAttr);
    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&threads[0], &pthreadAttr, receiver, this);
}

void Worker::Run(int request) {
    status = BUSY;
    result = core.calculate(request);
    status = ZOMBIE;
}

unsigned long long Worker::getResult() {
    if (status == ZOMBIE) {
        status = FREE;
        return result;
    }
    return -1;
}

Worker::~Worker() {
    MPI_Finalize();
}

void *Worker::receiver(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    while (!self->noTasks()) {
        pthread_mutex_lock(&self->mutex);
        self->DoOneTask();
        pthread_mutex_unlock(&self->mutex);
    }
    return nullptr;
}

void Worker::DoOneTask() {
    if (taskList.isEmpty()) {
        return;
    }
    int task = taskList.getFirstTask();
    store.addValue(std::to_string(task), std::to_string(core.calculate(task)));
}

bool Worker::noTasks() {
    return taskList.isEmpty();
}
