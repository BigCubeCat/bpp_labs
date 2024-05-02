#include <mpi.h>
#include <iostream>
#include "Worker.h"
#include "Core.h"

Worker::Worker(const Config &conf)
        : store(Storage(conf.storeSize)),
          useProfile(conf.calcDisbalance),
          useBalance(conf.useBalance),
          delay(conf.syncDelay),
          debug(conf.debug),
          bufferSize(conf.minimumCountTasks) {
    int threadProvided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &threadProvided);
    if (threadProvided != MPI_THREAD_MULTIPLE) { // если нельзя в многопоточность
        return;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    taskList.generateRandomList(mpiRank, conf.defaultCountTasks, conf.minTask, conf.maxTask);
    loadBalancer = LoadBalancer(mpiRank, mpiSize, conf.minimumCountTasks);

    swapBuff = new int[bufferSize];

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
    delete[] swapBuff;
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
    if (mpiRank != 0) return "";
    std::string result;
    auto dis = static_cast<int>(disbalance * 10000);
    if (useProfile) {
        auto value = std::to_string(dis / 100) + "." + std::to_string(dis % 100);
        result += "disbalance        " + value + "%\n";
    }
    result += "balanced           ";
    result += (useBalance) ? " true" : "false";
    result += "\n";
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
        if (self->debug)
            std::cout << "in rank " << self->mpiRank << " count tasks = " << self->taskList.countTasks() << std::endl;
    }
    if (self->useProfile) {
        endTime = MPI_Wtime();
        self->timeSpent = endTime - beginTime;
        if (self->debug)
            std::cout << self->mpiRank << " time = " << self->timeSpent << std::endl;
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
        if (self->useBalance) {
            // см README.md
            self->doBalance();
        }
        if (self->debug)
            std::cout << self->loadBalancer.toString() << std::endl;
    } while (self->loadBalancer.hasAnyTasks());
    // Вот тут надо дождаться, что все завершат последнюю операцию
    pthread_join(self->threads[1], nullptr);
    if (self->useProfile) {
        double maxTime, minTime;
        MPI_Allreduce(
                &self->timeSpent, &maxTime, 1,
                MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD
        );
        MPI_Allreduce(
                &self->timeSpent, &minTime, 1,
                MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD
        );
        self->disbalance = 1 - ((maxTime - minTime) / maxTime);
    }
    return nullptr;
}

void Worker::doBalance() {
    loadBalancer.balance();
    MPI_Bcast(loadBalancer.reassignments, mpiSize, MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << "reassigments = " << loadBalancer.reassignments[0] << " " << loadBalancer.reassignments[1] << " " << loadBalancer.reassignments[2] << " " << loadBalancer.reassignments[3] << std::endl;
    if (loadBalancer.reassignments[mpiRank] != -1) {
        if (loadBalancer.reassignments[mpiRank] == mpiRank) {
            // Получаем данные
            MPI_Recv(
                    swapBuff, bufferSize, MPI_INT,
                    MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
            readFromBuffer();
            if (debug)
                std::cout << "recv " << bufferSize << " tasks\n";
        } else {
            writeToBuffer();
            int dest = loadBalancer.reassignments[mpiRank];
            std::cout << "dest = " << dest << std::endl;
            MPI_Send(
                    swapBuff, bufferSize, MPI_INT, dest, 0, MPI_COMM_WORLD
            );
            if (debug)
                std::cout << "send " << bufferSize << " task to " << dest << "\n";
        }
    }
}

void Worker::writeToBuffer() {
    for (int i = 0; i < bufferSize; ++i) {
        swapBuff[i] = taskList.getLastTask();
    }
}

void Worker::readFromBuffer() {
    for (int i = 0; i < bufferSize; ++i) {
        taskList.addTask(swapBuff[i]);
    }
}
