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
          logger(Logger(rank, (conf.debug) ? DEBUG : WARN)) {
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

void Worker::fetchTasks(int rank, int count) {
    std::cout << "fetchTasks " << rank << " " << count << std::endl;
    swapBuff = new int[count];
    MPI_Recv(
            swapBuff, count, MPI_INT, rank,
            ASK_FOR_A_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );

    pthread_mutex_lock(&mem->mutex);
    core.loadTasks(count, swapBuff);
    pthread_mutex_unlock(&mem->mutex);
    delete[] swapBuff;

}

void Worker::giveTask(int rank) {
    int countTask = core.countTaskToDelegate();
    logger.debug("give " + std::to_string(countTask) + " to " + std::to_string(rank));
    MPI_Send(&countTask, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
    if (countTask > 0) {
        swapBuff = new int[countTask];
        core.dumpTasks(countTask, swapBuff);
        logger.debug("send " + std::to_string(countTask) + " to " + std::to_string(rank));
        MPI_Send(swapBuff, countTask, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
        delete[] swapBuff;
    }
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
        core.calculate();
        profiler.timeSpent += MPI_Wtime() - beginTime;
        if (config.useBalance) {
            if (core.needMoreTasks()) {
                logger.debug("call balancer");
                mem->flag = BALANCER; // Нужна балансировка
            }
        }
        pthread_mutex_unlock(&mem->mutex);
    }
}

void Worker::balancerThread() {
    // балансировщик должен запуститься после рабочего потока
    std::this_thread::sleep_for(std::chrono::milliseconds(config.syncDelay));
    while (mem->flag != END) {
        logger.debug("bb + " + std::to_string(mem->flag));
        if (mem->flag != BALANCER) {
            continue;
        }
        logger.debug("balancing");
        int rank, count = 0;
        bool processFound = false;
        for (int i = 1; i < mpiSize; ++i) {
            rank = (mpiRank + 1) % mpiSize;
            logger.debug(" try " + std::to_string(rank));
            // сообщаем, что мы хотим задачу и отправляем свой ранг
            MPI_Send(&mpiRank, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD);
            // получаем ответ - сколько задач нам готовы выдать
            MPI_Recv(&count, 1, MPI_INT, rank, ASK_FOR_A_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            logger.debug(" recieved answer from  " + std::to_string(rank) + " is " + std::to_string(count));
            // если задачи есть, то их надо получить
            if (count > 0) {
                logger.info("succes at " + std::to_string(rank));
                processFound = true;
                // получаем count задач
                logger.info("mutex lock ");
                fetchTasks(rank, count);
            }
        }
        // если у всех ничего - это конец
        pthread_mutex_lock(&mem->mutex);
        if (!processFound) {
            mem->flag = END;
        } else {
            mem->flag = WORKER;
        }
        pthread_mutex_unlock(&mem->mutex);
    }

    pthread_mutex_lock(&mem->mutex);
    getTiming();
    pthread_mutex_unlock(&mem->mutex);
}

void Worker::communicatorThread() {
    MPI_Request request;
    int flag, processRank;
    MPI_Status status;
    while (mem->flag != END) {
        logger.debug("comm");
        MPI_Irecv(
                &processRank, 1, MPI_INT, MPI_ANY_SOURCE,
                ASK_FOR_A_TASK, MPI_COMM_WORLD,
                &request
        );
        logger.debug("Irecv");
        std::this_thread::sleep_for(std::chrono::milliseconds(config.syncDelay));
        logger.debug("Testing");
        MPI_Test(&request, &flag, &status);
        logger.debug("response = " + std::to_string(flag));
        if (flag != 0) {
            pthread_mutex_lock(&mem->mutex);
            std::cout << "give the task! " << mpiRank << std::endl;
            giveTask(processRank);
            pthread_mutex_unlock(&mem->mutex);
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
    std::cout << "work thread ends " << self->mpiRank << std::endl;
    pthread_exit(nullptr);
    return nullptr;
}

void *Worker::balancerThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->balancerThread();
    std::cout << "balancer thread ends " << self->mpiRank << std::endl;
    pthread_exit(nullptr);
    return nullptr;
}

void *Worker::communicatorThreadFunction(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    if (!self) {
        return nullptr;
    }
    self->communicatorThread();
    std::cout << "comm thread ends " << self->mpiRank << std::endl;
    pthread_exit(nullptr);
    return nullptr;
}
