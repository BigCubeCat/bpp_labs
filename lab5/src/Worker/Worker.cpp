#include <mpi.h>
#include <iostream>
#include <climits>
#include <utility>
#include "Worker.h"
#include "Core.h"

const int ASK_FOR_A_TASK = INT_MAX;

Worker::Worker(int rank, int size, Core core, MutualMem *m, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          core(std::move(core)),
          mem(m),
          logger(Logger(rank, (conf.debug) ? DEBUG : ERROR)) {
    swapBuff = new int[config.swapSize];
    smolTimeout = conf.timeout / 2;
    processWorkload = new EProcessStatus[size];
    for (int i = 0; i < size; ++i) {
        processWorkload[i] = UNKNOWN;
    }
    processWorkload[mpiRank] = ME;
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
    delete[] processWorkload;
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
        // работа не должна блокироваться
        logger.info(" calculating " + std::to_string(core.countTasks()));
        core.calculate();
        core.print();
        pthread_mutex_lock(&mem->mutex);
        if (core.countTasks() <= 0) {
            mem->flag = END;
        }
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
    MPI_Request req[2];
    int flags[2], rank, first, last, countTasks;
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
            if (rank == mpiRank) continue;
            if (processWorkload[rank] == UNKNOWN) {
                countTasks = core.countTasks();
                MPI_Send(&countTasks, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
                MPI_Recv(&countTasks, 1, MPI_INT, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (countTasks == 0) {
                    logger.warn("received: no tasks");
                    processWorkload[rank] = NO_TASKS;
                    continue;
                }
                swapBuff = new int[countTasks];
                MPI_Recv(swapBuff, countTasks,
                         MPI_INT, rank, MPI_ANY_TAG,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
                logger.warn("received: success " + std::to_string(swapBuff[0]) + " " +
                            " from " + std::to_string(rank));
                pthread_mutex_lock(&mem->mutex);
                core.loadTasks(countTasks, swapBuff);
                pthread_mutex_unlock(&mem->mutex);
                delete[] swapBuff;

                processFound = true;
                break;
            }
        }
        // если у всех ничего - это конец
        pthread_mutex_lock(&mem->mutex);
        if (!processFound) {
            if (core.countTasks() == 0) mem->flag = END;
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
    int flag, rank, countTasks, myCountTasks, countTasksToSend;
    MPI_Status status;
    while (mem->flag != END) {
        MPI_Irecv(
                &countTasks, 1, MPI_INT, MPI_ANY_SOURCE,
                MPI_ANY_TAG, MPI_COMM_WORLD,
                &request
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(config.timeout * 2));
        MPI_Test(&request, &flag, &status);
        if (flag != 0) {
            rank = status.MPI_SOURCE;
            myCountTasks = core.countTasks();
            logger.debug(
                    "request from " + std::to_string(rank) +
                    "; countTasks = " + std::to_string(countTasks)
            );
            // если процесс просит задачу у нас, значит у него нечего брать
            processWorkload[status.MPI_SOURCE] = NO_TASKS;
            countTasksToSend = (myCountTasks - countTasks) / 2;
            if (myCountTasks <= config.critical || countTasksToSend <= 0) {
                int answer = 0;
                MPI_Send(&answer, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            } else {
                MPI_Send(&countTasksToSend, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
                swapBuff = new int[countTasksToSend];
                pthread_mutex_lock(&mem->mutex);
                core.dumpTasks(countTasksToSend, swapBuff);
                pthread_mutex_unlock(&mem->mutex);
                MPI_Send(swapBuff, countTasksToSend, MPI_INT, rank, 0, MPI_COMM_WORLD);
                logger.warn("send: success to " + std::to_string(rank));
                delete[] swapBuff;
            }
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
