#include <mpi.h>
#include <iostream>
#include <climits>
#include <utility>
#include "Worker.h"
#include "Core.h"

const int ASK_FOR_A_TASK = INT_MAX;
const int NO_TASKS = INT_MAX - 1;
const int ME = INT_MAX - 2;

Worker::Worker(int rank, int size, Core core, MutualMem *m, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          core(std::move(core)),
          mem(m),
          logger(Logger(rank, (conf.debug) ? DEBUG : ERROR)) {
    swapBuff = new int[config.swapSize];
    workload = new int[size];
    for (int i = 0; i < size; ++i) {
        workload[i] = ASK_FOR_A_TASK;
    }
    workload[mpiRank] = ME;
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

    pthread_join(threads[2], nullptr);
}

Worker::~Worker() {
    pthread_attr_destroy(&workThreadAttr);
    pthread_attr_destroy(&blncThreadAttr);
    pthread_attr_destroy(&commThreadAttr);
    pthread_mutex_destroy(&mem->mutex);
    delete[] swapBuff;
    delete[] workload;
}

std::string Worker::getResult() {
    if (mpiRank != 0) return "";
    std::string result = "rank               " + std::to_string(mpiRank) + "\n";
    result += "local time         " + std::to_string(profiler.timeSpent) + "\n";
    result += profiler.toString(config.defaultCountTasks, config.useBalance);
    return result;
}

void Worker::getTiming() {
    logger.debug("TIMING");
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
    logger.debug("end getTiming()");
}

void Worker::workerThread() {
    double beginTime = MPI_Wtime();
    while (mem->flag != END) {
        // logger.debug("worker, " + std::to_string(core.countTasks()) + ", flag = " + std::to_string(mem->flag));
        // работа не должна блокироваться
        logger.info(" calculating " + std::to_string(core.countTasks()));
        core.print();
        pthread_mutex_lock(&mem->mutex);
        if (core.countTasks() <= 0) {
            mem->flag = END;
        }
        core.calculate();
        pthread_mutex_unlock(&mem->mutex);
    }
    profiler.timeSpent = MPI_Wtime() - beginTime;
    pthread_mutex_lock(&mem->mutex);
    getTiming();
    pthread_mutex_unlock(&mem->mutex);
}

void Worker::balancerThread() {
    if (!config.useBalance) return;
    MPI_Status responseStatus;
    MPI_Request req;
    int flag, rank, first, last;
    bool processFound;
    while (mem->flag != END) {
        if (core.countTasks() > config.critical) {
            // задач еще много
            continue;
        }
        processFound = false;
        first = rand() % mpiSize;
        last = first + mpiSize;
        for (int i = first; i < last; ++i) {
            rank = i % mpiSize;
            if (workload[i] == ME) {
                continue;
            }
            if (workload[i] != ASK_FOR_A_TASK && workload[i] <= config.critical) {
                // мы уже знаем, что у него ничего нет
                logger.debug("i = " + std::to_string(i));
                continue;
            }
            // сообщаем, что мы хотим задачу и отправляем свой ранг
            MPI_Isend(&mpiRank, 1, MPI_INT, rank, core.countTasks(), MPI_COMM_WORLD, &req);
            std::this_thread::sleep_for(std::chrono::milliseconds(config.timeout));
            MPI_Test(&req, &flag, MPI_STATUS_IGNORE);
            if (flag == 0) { // если не дождались по таймаут - останавливаем запрос
                MPI_Cancel(&req);
                logger.warn("timeout");
                continue;
            }
            MPI_Recv(swapBuff, config.swapSize,
                     MPI_INT, rank, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &responseStatus
            );
            if (responseStatus.MPI_TAG == NO_TASKS) {
                logger.warn("received: no tasks");
            } else {
                logger.debug("tag = " + std::to_string(responseStatus.MPI_TAG));
                processFound = true;
                pthread_mutex_lock(&mem->mutex);
                logger.warn("received: success " + std::to_string(swapBuff[0]) + " " +
                            " from " + std::to_string(rank));
                core.loadTasks(config.swapSize, swapBuff);
                pthread_mutex_unlock(&mem->mutex);
                break;
            }
        }
        // если у всех ничего - это конец
        pthread_mutex_lock(&mem->mutex);
        if (!processFound && (core.countTasks() == 0)) {
            logger.debug("no more tasks, end");
            mem->flag = END;
        } else {
            logger.debug("more tasks, continue working");
            mem->flag = WORKER;
        }
        pthread_mutex_unlock(&mem->mutex);
    }

}

void Worker::communicatorThread() {
    if (!config.useBalance) return;
    MPI_Request request;
    int flag, processRank;
    MPI_Status status;
    while (mem->flag != END) {
        MPI_Irecv(
                &processRank, 1, MPI_INT, MPI_ANY_SOURCE,
                ASK_FOR_A_TASK, MPI_COMM_WORLD,
                &request
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(config.timeout / 3));
        MPI_Test(&request, &flag, &status);
        if (flag != 0) {
            workload[status.MPI_SOURCE] = status.MPI_TAG;
            logger.debug(
                    "request from " + std::to_string(processRank) + "; countTasks = " + std::to_string(status.MPI_TAG));
            if (core.countTasks() <= config.critical) {
                MPI_Send(swapBuff, config.swapSize, MPI_INT, processRank, NO_TASKS, MPI_COMM_WORLD);
                return;
            }
            logger.debug("swapBuff = ");
            for (int i = 0; i < config.swapSize; ++i) {
                std::cout << swapBuff[i] << " ";
            }
            std::cout << "\n";
            pthread_mutex_lock(&mem->mutex);
            core.dumpTasks(config.swapSize, swapBuff);
            pthread_mutex_unlock(&mem->mutex);
            logger.warn("sending: success to" + std::to_string(processRank));
            MPI_Send(swapBuff, config.swapSize, MPI_INT, processRank, core.countTasks(), MPI_COMM_WORLD);
        } else {
            MPI_Cancel(&request);
        }
    }
}

void *Worker::workerThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->workerThread();
    self->logger.info("end of worker thread");
    pthread_exit(nullptr);
    return nullptr;
}

void *Worker::balancerThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->balancerThread();
    self->logger.info("end of balancer thread");
    pthread_exit(nullptr);
    return nullptr;
}

void *Worker::communicatorThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->communicatorThread();
    self->logger.info("end of communicator thread");
    pthread_exit(nullptr);
    return nullptr;
}

int Worker::countTasksInProcess(int n, int rank, int size) {
    return n / size + (rank < n % size ? 1 : 0);
}

int Worker::firstLine(int n, int rank, int size) {
    int res = 0;
    for (int i = 0; i < rank; ++i) {
        res += countTasksInProcess(n, i, size);
    }
    return res;
}
