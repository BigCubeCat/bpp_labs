#include <iostream>
#include <mpi.h>
#include "MatrixModel/MatrixModel.h"
#include "FileWorker/FileWorker.h"


int main(int argc, char **argv) {
    int mpiRank, mpiSize;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    double startTime = MPI_Wtime();

    if (mpiRank == 0) {
        FileWorker fw = FileWorker("testdata/test1.matrix");
        auto calculationSetup = fw.readData();
        std::cout << "A matrix:\n";
        calculationSetup.matrixA->printMatrix();
        std::cout << "B matrix:\n";
        calculationSetup.matrixB->printMatrix();
    }

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
