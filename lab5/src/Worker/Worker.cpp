#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

Worker::Worker(int rank, int size, pthread_mutex_t *m, pthread_cond_t *cond, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          mutex(m),
          cond(cond),
          store(Storage(conf.storeSize)),
          loadBalancer(LoadBalancer(rank, size, conf.minimumCountTasks)),
          useBalance(conf.useBalance),
          delay(conf.syncDelay),
          debug(conf.debug) {
    taskList.generateRandomList(
            mpiRank, conf.defaultCountTasks,
            conf.minTask, conf.maxTask
    );

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
    pthread_mutex_destroy(mutex);
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
        pthread_mutex_lock(self->mutex);
        self->DoOneTask();
        pthread_mutex_unlock(self->mutex);
    }
    std::cout << "send signal in " << self->mpiRank << std::endl;
    pthread_cond_signal(self->cond);
    endTime = MPI_Wtime();

    self->timeSpent = endTime - beginTime;

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
//        pthread_mutex_lock(self->mutex);
        std::cout << self->taskList.countTasks() << std::endl;
        self->loadBalancer.updateCurrentCount(self->taskList.countTasks());
//        pthread_mutex_unlock(self->mutex);
        MPI_Allgather(
                myWorkloadPtr, 1, MPI_INT, self->loadBalancer.workload,
                1, MPI_INT, MPI_COMM_WORLD
        );
        if (self->useBalance && self->loadBalancer.needToBalance()) {
            // см README.md
            self->doBalance();
        }
        if (self->debug)
            std::cout << self->loadBalancer.toString() << std::endl;
    } while (self->loadBalancer.hasAnyTasks());
    // Вот тут надо дождаться, что все завершат последнюю операцию
    pthread_cond_wait(self->cond, self->mutex);
    self->getTiming();
    return nullptr;
}

void Worker::getTiming() {
    std::cout << "before barrier " << mpiRank << "\n";
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "find maximum\n";
    MPI_Allreduce(
            &timeSpent, &maxTime, 1,
            MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
    );
    std::cout << "find minimum\n";
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(
            &timeSpent, &minTime, 1,
            MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD
    );
    MPI_Barrier(MPI_COMM_WORLD);
    disbalance = ((maxTime - minTime) / maxTime);
    std::cout << "end of " << mpiRank << std::endl;
}

void Worker::doBalance() {
    loadBalancer.balance();
    MPI_Barrier(MPI_COMM_WORLD);
    pthread_mutex_lock(mutex);
    if (loadBalancer.reassignments[mpiRank] != -1) {
        int count = loadBalancer.counts[mpiRank];
        swapBuff = new int[count];
        if (loadBalancer.reassignments[mpiRank] == mpiRank) {
            // Получаем данные
            MPI_Recv(
                    swapBuff, count, MPI_INT,
                    MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
            readFromBuffer(count);
        } else {
            writeToBuffer(count);
            int dest = loadBalancer.reassignments[mpiRank];
            MPI_Send(swapBuff, count, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
        delete[] swapBuff;
    }
    pthread_mutex_unlock(mutex);
    MPI_Barrier(MPI_COMM_WORLD);
}

void Worker::writeToBuffer(int count) {
    taskList.dumpTasks(count, swapBuff);
}

void Worker::readFromBuffer(int count) {
    taskList.loadTasks(count, swapBuff);
}
