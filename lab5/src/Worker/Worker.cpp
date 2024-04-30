#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

Worker::Worker(int storeSize) : store(Storage(storeSize)) {
    taskList.addTask(19);
    int threadProvided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &threadProvided);
    if (threadProvided != MPI_THREAD_MULTIPLE) { // если нельзя в многопоточность
        return;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
    isMasterProcess = (mpiRank == 0);
    if (isMasterProcess) {
        loadBalancer = new LoadBalancer();
    }

    pthread_mutex_init(&mutex, nullptr);

    pthread_attr_init(&pthreadAttr);
    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&threads[0], &pthreadAttr, receiver, this);
    pthread_create(&threads[1], &pthreadAttr, calculator, this);
}

void Worker::Run() {
    pthread_join(threads[0], nullptr);
    pthread_join(threads[1], nullptr);
}

Storage Worker::getResult() {
    return store;
}

Worker::~Worker() {
    pthread_attr_destroy(&pthreadAttr);
    pthread_mutex_destroy(&mutex);
    MPI_Finalize();
}

void Worker::DoOneTask() {
    if (taskList.isEmpty()) {
        return;
    }
    int task = taskList.getFirstTask();
    int r = core.calculate(task);
    store.addValue(std::to_string(task), std::to_string(r));
}

bool Worker::noTasks() {
    return taskList.isEmpty();
}

void *Worker::calculator(void *ptr) {
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

void *Worker::receiver(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    // Коммуникация с мастером

    return nullptr;
}
