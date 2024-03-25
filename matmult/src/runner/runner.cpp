#include <mpi.h>
#include <iostream>
#include "./runner.h"
#include "../FileWorker/FileWorker.h"

TConfigStruct readFile(const std::string &filename) {
    FileWorker fw = FileWorker(filename);
    TConfigStruct calculationSetup = fw.readData();

    return calculationSetup
}

void RunMultiplication(const std::string &filename, int mpiRank, int mpiSize, bool debug) {
    int rootRowRank, rootColRank, rankComm2D, coordX, coordY;
    int dims[2] = {0, 0}, periods[2] = {0, 0}, coords[2];
    int n, k, m;
    bool isRoot = mpiRank == 0;

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

    if (isRoot) {
        auto calculationSetup = readFile(filename);
        n = calculationSetup.n;
        m = calculationSetup.m;
        k = calculationSetup.k;

        if (debug) {
            std::cout << "A:\n";
            calculationSetup.matrixA->printMatrix();
            std::cout << "B:\n";
            calculationSetup.matrixB->printMatrix();
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, comm2d);
    MPI_Bcast(&k, 1, MPI_INT, 0, comm2d);
    MPI_Bcast(&m, 1, MPI_INT, 0, comm2d);


}
