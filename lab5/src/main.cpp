#include <iostream>
#include <mpi.h>
#include "Config.h"
#include "Worker/Worker.h"
#include "Worker/MutualMem.h"

const int MAX_DIFF = 4;

void randomizeTask(int *arr, int size) {
    srand(time(nullptr));
    for (int i = 0; i < size; ++i) {
        arr[i] = (MAX_DIFF * (i + 1)) % 80;
    }
}

MutualMem mem;

int main(int argc, char **argv) {
    auto conf = Config(argc, argv);
    if (conf.debug) {
        std::cout << (conf.usingEnv ? "env" : "default") << std::endl;
    }
    int threadProvided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &threadProvided);
    if (threadProvided != MPI_THREAD_MULTIPLE) { // если нельзя в многопоточность
        return 1;
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    pthread_mutex_init(&mem.mutex, nullptr);

    int *counts = new int[size];
    int *disps = new int[size];
    for (int i = 0; i < size; ++i) {
        counts[i] = Worker::countTasksInProcess(conf.defaultCountTasks, i, size);
        disps[i] = Worker::firstLine(conf.defaultCountTasks, i, size);
    }

    int countTasks = Worker::countTasksInProcess(conf.defaultCountTasks, rank, size);
    int *inputArray = (rank == 0) ? new int[conf.defaultCountTasks] : new int[0];

    int *initialArray = new int[counts[rank]];
    if (rank == 0) {
        randomizeTask(inputArray, conf.defaultCountTasks);
    }

    MPI_Scatterv(
            inputArray, counts, disps,
            MPI_INT, initialArray, counts[rank],
            MPI_INT, 0, MPI_COMM_WORLD
    );

    auto core = Core(rank, initialArray, countTasks);
    auto worker = Worker(rank, size, core, &mem, conf);

    worker.Run();
    std::cout << worker.getResult() << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    delete[] counts;
    delete[] disps;
    delete[] initialArray;
    return 0;
}
