#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

Worker::Worker(int rank, int size, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          store(Storage(conf.storeSize)),
          loadBalancer(LoadBalancer(rank, size, conf.minimumCountTasks)),
          useBalance(conf.useBalance),
          delay(conf.syncDelay),
          debug(conf.debug) {

    taskList.generateRandomList(mpiRank, conf.defaultCountTasks, conf.minTask, conf.maxTask);

    swapBuff = 0;

    pthread_mutex_init(&mutex, nullptr);

    pthread_attr_init(&commThreadAttr);
    pthread_attr_init(&workThreadAttr);
    pthread_attr_setdetachstate(&commThreadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&workThreadAttr, PTHREAD_CREATE_JOINABLE);
}

void Worker::Run() {
    pthread_create(&threads[0], &commThreadAttr, communicatorThread, this);
    pthread_create(&threads[1], &workThreadAttr, workerThread, this);

    pthread_join(threads[0], nullptr);
}

Worker::~Worker() {
    pthread_attr_destroy(&workThreadAttr);
    pthread_attr_destroy(&commThreadAttr);
    pthread_mutex_destroy(&mutex);
}

void Worker::DoOneTask() {
    if (taskList.isEmpty()) {
        return;
    }
    int task = taskList.getFirstTask();
    store.addValue(std::to_string(task), Core::calculate(task));
}

bool Worker::noTasks() {
    return taskList.isEmpty();
}

std::string Worker::getResult() const {
    if (mpiRank != 0) return "";
    auto dis = static_cast<int>(disbalance * 10000);
    auto value = std::to_string(dis / 100) + "." + std::to_string(dis % 100);
    std::string result;
    result += "count tasks        " + std::to_string(config.defaultCountTasks) + "\n";
    result += "maximum time       " + std::to_string(maxTime) + "\n";
    result += "minimum time       " + std::to_string(minTime) + "\n";
    result += "disbalance         " + value + "%\n";
    result += "balanced           ";
    result += (useBalance) ? " true" : "false";
    result += "\n";
    return result;
}

void *Worker::workerThread(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    double beginTime = 0, endTime = 0;
    if (!self) {
        return nullptr;
    }
    beginTime = MPI_Wtime();
    while (!self->noTasks()) {
        pthread_mutex_lock(&self->mutex);
        self->DoOneTask();
        pthread_mutex_unlock(&self->mutex);
    }
    endTime = MPI_Wtime();

    pthread_mutex_lock(&self->mutex);
    self->timeSpent = endTime - beginTime;
    pthread_mutex_unlock(&self->mutex);

    if (self->debug)
        std::cout << self->mpiRank << " time = " << self->timeSpent << std::endl;
    return nullptr;
}

void *Worker::communicatorThread(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    // указатель на наш элемент в массиве
    int *myWorkloadPtr = self->loadBalancer.currentWorkload();
    do {
        // ничего не делаем :|
        std::this_thread::sleep_for(std::chrono::milliseconds(self->delay));
        // узнаем, сколько у нас осталось задач
        self->loadBalancer.updateCurrentCount(self->taskList.countTasks());
        MPI_Allgather(
                myWorkloadPtr, 1, MPI_INT, self->loadBalancer.workload,
                1, MPI_INT, MPI_COMM_WORLD
        );
        if (self->useBalance) {
            // см README.md
            self->doBalance();
        }
        if (self->debug)
            std::cout << self->loadBalancer.toString() << std::endl;
    } while (self->loadBalancer.hasAnyTasks());
    // Вот тут надо дождаться, что все завершат последнюю операцию
    MPI_Barrier(MPI_COMM_WORLD);
    pthread_join(self->threads[0], nullptr);
    MPI_Allreduce(
            &self->timeSpent, &self->maxTime, 1,
            MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
    );
    MPI_Allreduce(
            &self->timeSpent, &self->minTime, 1,
            MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD
    );
    self->disbalance = ((self->maxTime - self->minTime) / self->maxTime);
    return nullptr;
}

void Worker::doBalance() {
    loadBalancer.balance();
    MPI_Bcast(loadBalancer.reassignments, mpiSize, MPI_INT, 0, MPI_COMM_WORLD);
    if (loadBalancer.reassignments[mpiRank] != -1) {
        if (loadBalancer.reassignments[mpiRank] == mpiRank) {
            // Получаем данные
            MPI_Recv(
                    &swapBuff, 1, MPI_INT,
                    MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
            readFromBuffer();
        } else {
            writeToBuffer();
            int dest = loadBalancer.reassignments[mpiRank];
            MPI_Send(&swapBuff, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    }
}

void Worker::writeToBuffer() {
    swapBuff = taskList.getLastTask();
}

void Worker::readFromBuffer() {
    taskList.addTask(swapBuff);
}
