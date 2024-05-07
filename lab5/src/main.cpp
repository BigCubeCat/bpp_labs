#include <iostream>
#include <mpi.h>
#include "Config.h"
#include "Worker/Worker.h"
#include "Worker/MutualMem.h"

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
    pthread_cond_init(&mem.cond, nullptr);

    auto worker = Worker(rank, size, &mem, conf);
    worker.Run();
    std::cout << worker.getResult();

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
