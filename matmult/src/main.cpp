#include <iostream>
#include <mpi.h>
#include "MatrixModel/MatrixModel.h"
#include "FileWorker/FileWorker.h"
#include "CalculationController/CalculationController.h"


int main(int argc, char **argv) {
    int mpiRank, mpiSize, rootRowRank, rootColRank, rankComm2D, coordX, coordY;
    int dims[2] = {0, 0}, periods[2] = {0, 0}, coords[2];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    MPI_Comm comm2d;
    MPI_Dims_create(mpiSize, 2, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm2d);

    MPI_Comm_rank(comm2d, &rankComm2D);
    MPI_Cart_get(comm2d, 2, dims, periods, coords);
    coordY = coords[0];
    coordX = coords[1];
    MPI_Cart_coords(comm2d, 0, 2, coords);
    rootRowRank = coords[0];
    rootColRank = coords[1];

    double startTime = MPI_Wtime();
    TConfigStruct calculationSetup;
    if (mpiRank == 0) {
        FileWorker fw = FileWorker("testdata/test1.matrix");
        calculationSetup = fw.readData();
        std::cout << "A matrix:\n";
        calculationSetup.matrixA->printMatrix();
        std::cout << "B matrix:\n";
        calculationSetup.matrixB->printMatrix();
        CalculationController calculator = CalculationController(calculationSetup, mpiRank, mpiSize);
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
