#include <mpi.h>
#include <iostream>
#include "./runner.h"
#include "../FileWorker/FileWorker.h"
#include "setup.h"

TConfigStruct readFile(const std::string &filename, int &n, int &m, int &k) {
    FileWorker fw = FileWorker(filename);
    TConfigStruct calculationSetup = fw.readData();

    n = calculationSetup.n;
    m = calculationSetup.m;
    k = calculationSetup.k;

    return calculationSetup;
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

    int *firstLines = new int[dims[0]];
    int *linesCount = new int[dims[0]];
    int *firstColumns = new int[dims[1]];
    int *columnsCount = new int[dims[1]];
    setupLines(firstLines, linesCount, firstColumns, columnsCount, dims, n, k);

    MPI_Datatype rowType, columnType, wideLongCell, wideShortCell, narrowLongCell, narrowShortCell;
    setupDatatypes(
            &rowType, &columnType,
            &wideLongCell, &wideShortCell, &narrowLongCell, &narrowShortCell,
            dims, n, m, k
    );

    std::cout << rowType << std::endl;

    auto *resultMat = new MatrixModel(linesCount[coordY], columnsCount[coordX]);

    auto horizontalStrip = new double[linesCount[coordY] * m];
    auto verticalStrip = new double[m * columnsCount[coordX]];

    // Рассылаем строки по нулевому столбцу
    if (coordX == 0) {
        MPI_Scatterv(
                (mpiRank == 0 ? calculationSetup.matrixA->data : nullptr),
                linesCount, firstLines,
                rowType, horizontalStrip, linesCount[coordX],
                rowType, 0, columnCommunicator
        );
    }
    MPI_Bcast(horizontalStrip, linesCount[coordY], rowType, rootColRank, rowCommunicator);

    delete[] firstLines;
    delete[] linesCount;
    delete[] firstColumns;
    delete[] columnsCount;

    delete resultMat;
    delete[] horizontalStrip;
    delete[] verticalStrip;
}
