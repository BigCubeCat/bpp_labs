#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

Worker::Worker(const Config &conf)
        : store(Storage(conf.storeSize)),
          useProfile(conf.calcDisbalance),
          useBalance(conf.useBalance),
          delay(conf.syncDelay),
          debug(conf.debug) {
    int threadProvided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &threadProvided);
    if (threadProvided != MPI_THREAD_MULTIPLE) { // если нельзя в многопоточность
        return;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    taskList.generateRandomList(mpiRank, conf.defaultCountTasks, conf.minTask, conf.maxTask);
    loadBalancer = LoadBalancer(mpiRank, mpiSize, conf.minimumCountTasks);

    pthread_mutex_init(&mutex, nullptr);

    pthread_attr_init(&pthreadAttr);
    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&threads[0], &pthreadAttr, communicator, this);
    pthread_create(&threads[1], &pthreadAttr, calculator, this);
}

void Worker::Run() {
    pthread_join(threads[0], nullptr);
    pthread_join(threads[1], nullptr);
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
    store.addValue(std::to_string(task), Core::calculate(task));
}

bool Worker::noTasks() {
    return taskList.isEmpty();
}

std::string Worker::getResult() {
    std::string result = "------------------------\n";
    result += "rank\t\t" + std::to_string(mpiRank) + "\n";
    if (useProfile)
        result += "disbalance\t" + std::to_string(disbalance) + "\n";
    result += "balanced\t";
    result += (useBalance) ? "true" : "false";
    result += "\n------------------------\n";
    return result;
}

void *Worker::calculator(void *ptr) {
    auto *self = static_cast<Worker *>(ptr);
    double beginTime, endTime;
    if (!self) {
        return nullptr;
    }
    if (self->useProfile) beginTime = MPI_Wtime();
    while (!self->noTasks()) {
        self->DoOneTask();
        if (self->debug) {
            std::cout << "in rank " << self->mpiRank << " count tasks = " << self->taskList.countTasks() << std::endl;
        }
    }
    self->imDone = true;
    if (self->useProfile) {
        endTime = MPI_Wtime();
        self->timeSpent = endTime - beginTime;
    }
    return nullptr;
}

void *Worker::communicator(void *ptr) {
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
        // дейли митинг для бедных
        MPI_Allgather(
                myWorkloadPtr, 1, MPI_INT, self->loadBalancer.workload, 1, MPI_INT, MPI_COMM_WORLD
        );
        if (self->debug)
            std::cout << self->loadBalancer.toString() << std::endl;
    } while (self->loadBalancer.hasAnyTasks() || !self->imDone);
    if (self->useProfile) {
        double maxTime;
        MPI_Allreduce(
                &self->timeSpent, &maxTime, 1,
                MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
        );
        self->disbalance = 1 - (self->timeSpent / maxTime);
    }
    return nullptr;
}
