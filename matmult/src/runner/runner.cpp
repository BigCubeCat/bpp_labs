#include <mpi.h>
#include <iostream>
#include "./runner.h"
#include "../FileWorker/FileWorker.h"
#include "../algo/algo.h"

TConfigStruct readFile(const std::string &filename, int &n, int &m, int &k) {
    FileWorker fw = FileWorker(filename);
    TConfigStruct calculationSetup = fw.readData();

    n = calculationSetup.n;
    m = calculationSetup.m;
    k = calculationSetup.k;

    return calculationSetup;
}

void printArr(int *arr, int size) {
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
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

    MPI_Comm rowCommunicator, columnCommunicator;
    MPI_Comm_split(comm2d, coordY, coordX, &rowCommunicator);
    MPI_Comm_split(comm2d, coordX, coordY, &columnCommunicator);

    TConfigStruct calculationSetup;
    if (isRoot) {
        calculationSetup = readFile(filename, n, m, k);
        if (debug) calculationSetup.print();
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Datatype largeRowType;
    MPI_Type_contiguous(m, MPI_DOUBLE, &largeRowType);
    MPI_Type_commit(&largeRowType);

    int *linesCount = new int[dims[0]];
    int *firstLines = new int[dims[0]];
    int *columnsCount = new int[dims[1]];
    int *firstColumns = new int[dims[1]];

    std::cout << "n = " << n << std::endl;
    for (int i = 0; i < dims[0]; ++i) {
        linesCount[i] = countOfLines(n, i, dims[0]);
        firstLines[i] = firstLine(n, i, dims[0]);
    }

    for (int i = 0; i < dims[1]; ++i) {
        columnsCount[i] = countOfLines(k, i, dims[1]);
        firstColumns[i] = firstLine(k, i, dims[1]);
    }

    if (debug && mpiRank == 0) {
        std::cout << "linesCount = " << std::endl;
        printArr(linesCount, dims[0]);
        std::cout << "firstLines = " << std::endl;
        printArr(firstLines, dims[0]);

        std::cout << "columnsCount = " << std::endl;
        printArr(columnsCount, dims[1]);
        std::cout << "firstColumns = " << std::endl;
        printArr(firstColumns, dims[1]);
    }

    MatrixModel horizontalStrip = MatrixModel(linesCount[coordY], m);
    MatrixModel verticalStrip = MatrixModel(m, columnsCount[coordX]);

    if (mpiRank == 0) {
        horizontalStrip.data = calculationSetup.matrixA->data;
        for (int i = 1; i < dims[0]; ++i) {
            MPI_Send(calculationSetup.matrixA->data + firstLines[i] * m, linesCount[i] * m, MPI_DOUBLE, i, 0,
                     columnCommunicator);
        }
    } else if (coordX == 0) {
        MPI_Recv(horizontalStrip.data, linesCount[coordY] * m, MPI_DOUBLE, 0, 0, columnCommunicator, MPI_STATUS_IGNORE);
    }
    MPI_Bcast(horizontalStrip.data, linesCount[coordY] * m, MPI_DOUBLE, rootRowRank, rowCommunicator);

    std::cout << mpiRank << "strip = ";
    horizontalStrip.printMatrix();


    delete[] linesCount;
    delete[] firstLines;
}
