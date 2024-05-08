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
    pthread_attr_setdetachstate(&commThreadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&blncThreadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate(&workThreadAttr, PTHREAD_CREATE_JOINABLE);
}

void Worker::Run() {
    pthread_create(&threads[0], &commThreadAttr, communicatorThreadFunction, this);
    pthread_create(&threads[1], &blncThreadAttr, balancerThreadFunction, this);
    pthread_create(&threads[2], &workThreadAttr, workerThreadFunction, this);

    pthread_join(threads[0], nullptr);
    pthread_join(threads[1], nullptr);
    pthread_join(threads[2], nullptr);
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

void Worker::getTiming() {
    std::cout << "TIMING " << mpiRank << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "afterBarier " << mpiRank << std::endl;
    MPI_Allreduce(
            &profiler.timeSpent, &profiler.maxTime, 1,
            MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
    );
    std::cout << "afterAllreduce " << mpiRank << std::endl;
    MPI_Allreduce(
            &profiler.timeSpent, &profiler.minTime, 1,
            MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD
    );
    std::cout << "afterAllreduce second " << mpiRank << std::endl;
    profiler.calcDisbalance();
}

void Worker::fetchTasks(int rank, int count) {
    std::cout << "fetchTasks " << rank << " " << count << std::endl;
    swapBuff = new int[count];
    MPI_Recv(
            swapBuff, count, MPI_INT, rank,
            ASK_FOR_A_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
    core.loadTasks(count, swapBuff);
    delete[] swapBuff;

}

void Worker::giveTask(int rank) {
    std::cout << "giveTask " << rank << std::endl;
    int countTask = core.countTaskToDelegate();
    MPI_Send(&countTask, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
    if (countTask > 0) {
        swapBuff = new int[countTask];
        core.dumpTasks(countTask, swapBuff);
        MPI_Send(swapBuff, countTask, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
        delete[] swapBuff;
    }
}

void Worker::workerThread() {
    double beginTime = 0;
    profiler.timeSpent = 0;
    while (mem->flag != END){
        if (mem->flag != WORKER) {
            continue;
        }
        beginTime = MPI_Wtime();
        pthread_mutex_lock(&mem->mutex);
        std::cout << mpiRank << " calculating " << core.countTasks() << std::endl;
        core.calculate();
        profiler.timeSpent += MPI_Wtime() - beginTime;
        if (core.needMoreTasks()) {
            std::cout << "call balancer\n";
            mem->flag = BALANCER; // Нужна балансировка
        }
        pthread_mutex_unlock(&mem->mutex);
    }
    pthread_exit(nullptr);
}

void Worker::balancerThread() {
    // балансировщик должен запуститься после рабочего потока
    std::this_thread::sleep_for(std::chrono::milliseconds(config.syncDelay));
    while(mem->flag != END) {
        if (mem->flag != BALANCER) {
            continue;
        }
        int rank, count = 0;
        bool processFound = false;
        for (int i = mpiRank + 1; i < mpiSize - 1; ++i) {
            rank = i % mpiSize;
            // сообщаем, что мы хотим задачу и отправляем свой ранг
            MPI_Send(&mpiRank, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
            // получаем ответ - сколько задач нам готовы выдать
            MPI_Recv(&count, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // если задачи есть, то их надо получить
            if (count > 0) {
                processFound = true;
                // получаем count задач
                pthread_mutex_lock(&mem->mutex);
                fetchTasks(rank, count);
                pthread_mutex_unlock(&mem->mutex);
            }
        }
        if (!processFound) {
            // если у всех ничего - это конец
            pthread_mutex_lock(&mem->mutex);
            std::cout << "END " << mpiRank << std::endl;
            mem->flag = END;
            pthread_mutex_unlock(&mem->mutex);
        }
    }

    pthread_mutex_lock(&mem->mutex);
    getTiming();
    pthread_mutex_unlock(&mem->mutex);

    pthread_exit(nullptr);
}

void Worker::communicatorThread() {
    MPI_Request request;
    int flag, processRank;
    MPI_Status status;
    while (mem->flag != END) {
        std::cout << mpiRank << " comm\n";
        std::this_thread::sleep_for(std::chrono::seconds(config.syncDelay)); // надо ли это?
        MPI_Irecv(
                &processRank, 1, MPI_INT, MPI_ANY_SOURCE,
                ASK_FOR_A_TASK, MPI_COMM_WORLD,
                &request
        );
        std::this_thread::sleep_for(std::chrono::seconds(config.syncDelay)); // надо ли это?
        MPI_Test(&request, &flag, &status);
        if (flag) {
            pthread_mutex_lock(&mem->mutex);
            giveTask(processRank);
            pthread_mutex_unlock(&mem->mutex);
        } else {
            MPI_Cancel(&request);
        }
    }
    pthread_exit(nullptr);
}

void *Worker::workerThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->workerThread();
    return nullptr;
}

void *Worker::balancerThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->balancerThread();
    return nullptr;
}

void *Worker::communicatorThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->communicatorThread();
    return nullptr;
}
