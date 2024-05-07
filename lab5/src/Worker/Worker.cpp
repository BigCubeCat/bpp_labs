#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

const int ASK_FOR_A_TASK = 1;

Worker::Worker(int rank, int size, MutualMem *m, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          core(Core(rank, conf.defaultCountTasks, conf.minTask, conf.maxTask)),
          mem(m),
          debug(conf.debug) {
    pthread_attr_init(&commThreadAttr);
    pthread_attr_init(&blncThreadAttr);
    pthread_attr_init(&workThreadAttr);
    // pthread_attr_setdetachstate(&commThreadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&blncThreadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&workThreadAttr, PTHREAD_CREATE_JOINABLE);
}

void Worker::Run() {
    // pthread_create(&threads[0], &commThreadAttr, communicatorThread, this);
    pthread_create(&threads[1], &blncThreadAttr, balancerThread, this);
    pthread_create(&threads[2], &workThreadAttr, workerThread, this);

    // pthread_join(threads[0], nullptr);
    pthread_join(threads[2], nullptr);
    pthread_join(threads[1], nullptr);
}

Worker::~Worker() {
    pthread_attr_destroy(&workThreadAttr);
    pthread_attr_destroy(&blncThreadAttr);
    pthread_attr_destroy(&commThreadAttr);
    pthread_mutex_destroy(&mem->mutex);
}

std::string Worker::getResult() {
    if (mpiRank != 0) return "";
    return profiler.toString(config.defaultCountTasks, config.useBalance);
}

void *Worker::workerThread(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }

    std::cout << "workerThread " << self->mpiRank << std::endl;
    double beginTime = 0;
    self->profiler.timeSpent = 0;
    do {
        pthread_mutex_lock(&self->mem->mutex);
        std::cout << "worker " << self->mpiRank << std::endl;
        beginTime = MPI_Wtime();
        self->core.calculate();
        self->profiler.timeSpent += MPI_Wtime() - beginTime;
        if (!self->core.isBusy()) {
            pthread_cond_signal(&self->mem->cond);
        }
        pthread_mutex_unlock(&self->mem->mutex);
    } while (self->mem->flag != END);

    return nullptr;
}

void *Worker::balancerThread(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    std::this_thread::sleep_for(std::chrono::seconds(self->config.syncDelay));
    std::cout << "balancerThread " << self->mpiRank << std::endl;
    pthread_mutex_lock(&self->mem->mutex);
    pthread_cond_wait(&self->mem->cond, &self->mem->mutex);
    std::cout << "балансировка \n";
    //self->askForTask(); // обрабатываем отсутствие задач
    pthread_mutex_unlock(&self->mem->mutex);

    return nullptr;
}

void *Worker::communicatorThread(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }

    MPI_Request request;
    int flag;
    MPI_Status status;

    int processRank;
    while (self->mem->flag != END) {
        MPI_Irecv(
                &processRank, 1, MPI_INT, MPI_ANY_SOURCE,
                ASK_FOR_A_TASK, MPI_COMM_WORLD,
                &request
        );
        std::this_thread::sleep_for(std::chrono::seconds(self->config.syncDelay));
        MPI_Test(&request, &flag, &status);

        if (flag == 0) {
            self->giveTask(processRank);
        } else {
            MPI_Cancel(&request);
        }
    }
    return nullptr;
}

void Worker::getTiming() {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(
            &profiler.timeSpent, &profiler.maxTime, 1,
            MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
    );
    MPI_Allreduce(
            &profiler.timeSpent, &profiler.minTime, 1,
            MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD
    );
    profiler.calcDisbalance();
}

void Worker::askForTask() {
    int rank, count = 0;
    for (int i = mpiRank + 1; i < mpiSize - 1; ++i) {
        rank = i % mpiSize;
        if (debug) std::cout << mpiRank << " asking for a task " << rank << std::endl;
        MPI_Send(&mpiRank, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
        MPI_Recv(&count, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (count > 0) {
            swapBuff = new int[count];
            MPI_Recv(
                    swapBuff, count, MPI_INT, rank,
                    ASK_FOR_A_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
            core.loadTasks(count, swapBuff);
            delete[] swapBuff;
            return;
        }
    }
}

void Worker::giveTask(int rank) {
    int countTask = core.countTaskToDelegate();
    if (debug) std::cout << mpiRank << " gives " << countTask << " to " << rank << std::endl;
    MPI_Send(&countTask, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
    if (countTask > 0) {
        swapBuff = new int[countTask];
        core.dumpTasks(countTask, swapBuff);
        MPI_Send(swapBuff, countTask, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
        delete[] swapBuff;
    }
}
