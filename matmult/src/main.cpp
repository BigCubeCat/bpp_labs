#include <iostream>
#include <mpi.h>
#include "runner/runner.h"


int main(int argc, char **argv) {
    int mpiRank, mpiSize;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    std::string filename = argv[1];
    bool debug = false;
    if (argc == 3) {
        debug = std::string(argv[2]) == "debug";
    }
    double startTime = MPI_Wtime();

    RunMultiplication(filename, mpiRank, mpiSize, debug);

    // через MPI_Vector_type разрезать на колонки.
    // должно быть несколько типов: причем некоторые колонки шире других

    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;

    double maxTime;
    MPI_Reduce(&elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);
    if (mpiRank == 0)
        std::cout << maxTime << std::endl;

    MPI_Finalize();
    return 0;
}
