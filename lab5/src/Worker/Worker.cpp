#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

const int ASK_FOR_A_TASK = 1;
const int NO_TASKS = 2;

Worker::Worker(int rank, int size, MutualMem *m, const Config &conf)
        : mpiRank(rank),
          mpiSize(size),
          config(conf),
          core(Core(rank, conf.defaultCountTasks, conf.minTask, conf.maxTask)),
          mem(m),
          logger(Logger(rank, (conf.debug) ? DEBUG : WARN)) {
    swapBuff = new int[config.minimumCountTasks];
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
}

std::string Worker::getResult() {
    if (mpiRank != 0) return "";
    return profiler.toString(config.defaultCountTasks, config.useBalance);
}

void Worker::getTiming() {
    logger.debug("TIMING");
    MPI_Barrier(MPI_COMM_WORLD);
    logger.debug("after barier");
    MPI_Allreduce(
            &profiler.timeSpent, &profiler.maxTime, 1,
            MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
    );
    logger.debug("after barier");
    MPI_Allreduce(
            &profiler.timeSpent, &profiler.minTime, 1,
            MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD
    );
    profiler.calcDisbalance();
    logger.debug("end getTiming()");
}

void Worker::workerThread() {
    double beginTime = 0;
    profiler.timeSpent = 0;
    while (mem->flag != END) {
        // logger.debug("worker, " + std::to_string(core.countTasks()) + ", flag = " + std::to_string(mem->flag));
        // работа не должна блокироваться
        beginTime = MPI_Wtime();
        logger.info(" calculating " + std::to_string(core.countTasks()));
        pthread_mutex_lock(&mem->mutex);
        if (core.countTasks() == 0) {
            mem->flag = END;
        }
        core.calculate();
        profiler.timeSpent += MPI_Wtime() - beginTime;
        pthread_mutex_unlock(&mem->mutex);
    }

    pthread_mutex_lock(&mem->mutex);
    getTiming();
    pthread_mutex_unlock(&mem->mutex);
}

void Worker::balancerThread() {
    MPI_Status responseStatus;
    // балансировщик должен запуститься после рабочего потока
    std::this_thread::sleep_for(std::chrono::milliseconds(config.syncDelay));
    while (mem->flag != END) {
        if (!core.needMoreTasks()) {
            // нет смысла сразу проверять, так на необходимость балансировки наш процесс
            // std::this_thread::sleep_for(std::chrono::milliseconds(config.syncDelay));
            continue;
        }
        int rank;
        bool processFound = false;
        for (int i = 1; i < mpiSize; ++i) {
            rank = (mpiRank + 1) % mpiSize;
            logger.debug(" try " + std::to_string(rank));
            // сообщаем, что мы хотим задачу и отправляем свой ранг
            MPI_Send(&mpiRank, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
            // получаем ответ - сколько задач нам готовы выдать
            MPI_Recv(swapBuff, config.minimumCountTasks,
                     MPI_INT, rank, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &responseStatus
            );
            if (responseStatus.MPI_TAG == NO_TASKS) {
                logger.warn("received: no tasks");
            } else {
                processFound = true;
                pthread_mutex_lock(&mem->mutex);
                logger.warn("received: success " + std::to_string(swapBuff[0]) + " " + std::to_string(swapBuff[1]) + " from " + std::to_string(rank));
                core.loadTasks(config.minimumCountTasks, swapBuff);
                pthread_mutex_unlock(&mem->mutex);
            }
        }
        // если у всех ничего - это конец
        pthread_mutex_lock(&mem->mutex);
        if (!processFound && (core.countTasks() == 0)) {
            logger.debug("no more tasks, end");
            mem->flag = END;
        } else {
            logger.debug("no more tasks, continue working");
            mem->flag = WORKER;
        }
        pthread_mutex_unlock(&mem->mutex);
    }

}

void Worker::communicatorThread() {
    MPI_Request request;
    int flag, processRank, countTask;
    MPI_Status status;
    while (mem->flag != END) {
        MPI_Irecv(
                &processRank, 1, MPI_INT, MPI_ANY_SOURCE,
                ASK_FOR_A_TASK, MPI_COMM_WORLD,
                &request
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(config.syncDelay));
        MPI_Test(&request, &flag, &status);
        if (flag != 0) {
            countTask = core.countTaskToDelegate();
            logger.debug("request from " + std::to_string(processRank) + "; countTasks = " + std::to_string(countTask));
            if (countTask < config.minimumCountTasks) {
                MPI_Send(swapBuff, countTask, MPI_INT, processRank, NO_TASKS, MPI_COMM_WORLD);
                return;
            }
            pthread_mutex_lock(&mem->mutex);
            core.dumpTasks(countTask, swapBuff);
            pthread_mutex_unlock(&mem->mutex);
            logger.warn("sending: success " + std::to_string(swapBuff[0]) + " " + std::to_string(swapBuff[1]));
            MPI_Send(swapBuff, config.minimumCountTasks, MPI_INT, processRank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
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
