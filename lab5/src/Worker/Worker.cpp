#include <mpi.h>
#include <iostream>
#include <utility>
#include "Worker.h"
#include "Core.h"

const int ASK_FOR_A_TASK = 128;
const int ACCEPT_TASKS = 256;

Worker::Worker(int rank, int size, Core core, MutualMem *m, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          core(std::move(core)),
          mem(m),
          logger(Logger(rank, (conf.debug) ? INFO : ERROR)) {
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
    std::string result = "local time         " + std::to_string(profiler.timeSpent) + "\n";
    result += profiler.toString(config.defaultCountTasks, config.useBalance);
    return result;
}

void Worker::getTiming() {
    logger.info("TIMING");
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
    logger.info("end getTiming()");
}

void Worker::workerThread() {
    double beginTime = MPI_Wtime();
    while (mem->flag != END) {
        // работа не должна блокироваться
        logger.info(" calculating " + std::to_string(core.countTasks()));
        core.calculate();
        if (config.debug) core.print();
        pthread_mutex_lock(&mem->mutex);
        if (core.countTasks() <= 0) {
            // Ожидается, что балансировщик работает эффеткивно
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
    int rank, first, last;
    bool allProcessVisited;
    while (mem->flag != END) {
        if (core.countTasks() > config.critical) {
            // задач еще много
            continue;
        }
        first = (mpiRank + 1) % mpiSize;
        last = first + mpiSize;
        allProcessVisited = true;
        for (int i = first; i < last; ++i) {
            rank = i % mpiSize;
            if (rank == mpiRank) continue;
            if (processWorkload[rank] == UNKNOWN) {
                allProcessVisited = false;
                MPI_Send(swapBuff, config.swapSize, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
                processWorkload[rank] = NO_TASKS;
            }
        }
        if (allProcessVisited) {
            for (int i = 0; i < mpiSize; ++i) {
                processWorkload[i] = UNKNOWN;
            }
        }
    }
}

void Worker::communicatorThread() {
    if (!config.useBalance) return;
    MPI_Request request;
    int flag, rank, myCountTasks, diff, tag;
    MPI_Status status;
    while (mem->flag != END) {
        MPI_Irecv(
                swapBuff, config.swapSize, MPI_INT,
                MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
                &request
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(config.timeout));
        if (mem->flag == END) break; // Если процесс завершился, а мы это проспали
        MPI_Test(&request, &flag, &status);
        if (flag != 0) {
            rank = status.MPI_SOURCE;
            tag = status.MPI_TAG;
            // либо нам уже все отдали, либо у нас просят
            processWorkload[status.MPI_SOURCE] = NO_TASKS;
            if (tag == ACCEPT_TASKS) { // принимаем задачи
                logger.info("accept: success from " + std::to_string(rank));
                pthread_mutex_lock(&mem->mutex);
                core.loadTasks(config.swapSize, swapBuff);
                pthread_mutex_unlock(&mem->mutex);
            } else if (tag == ASK_FOR_A_TASK) {
                myCountTasks = core.countTasks();
                // если процесс просит задачу у нас, значит у него нечего брать
                diff = myCountTasks - config.swapSize;
                if (diff <= 0) {
                    MPI_Send(swapBuff, config.swapSize, MPI_INT, rank, 0, MPI_COMM_WORLD);
                } else {
                    pthread_mutex_lock(&mem->mutex);
                    core.dumpTasks(config.swapSize, swapBuff);
                    pthread_mutex_unlock(&mem->mutex);
                    MPI_Send(swapBuff, config.swapSize, MPI_INT, rank, ACCEPT_TASKS, MPI_COMM_WORLD);
                    logger.info("send: success to " + std::to_string(rank));
                }
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
