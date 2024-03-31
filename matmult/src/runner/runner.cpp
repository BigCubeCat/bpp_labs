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
    MPI_Comm comm2d, rowCommunicator, columnCommunicator;
    int rootRowRank, rootColRank, rankComm2D, coordX, coordY, n, k, m, dims[2] = {0, 0};
    bool isRoot = mpiRank == 0;
    setupComm(comm2d, rowCommunicator, columnCommunicator, coordX, coordY, rootColRank, rootRowRank, dims, mpiSize);

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

    auto horizontalStrip = MatrixModel(linesCount[coordY], m);
    auto verticalStrip = MatrixModel(columnsCount[coordX], m);

    // Рассылаем строки по нулевому столбцу
    if (coordX == rootColRank) {
        MPI_Scatterv(
                (isRoot ? calculationSetup.matrixA->data : nullptr),
                linesCount, firstLines,
                rowType, horizontalStrip.data, linesCount[coordY],
                rowType, rootColRank, columnCommunicator
        );
    }
    // Рассылаем колонки по нулевой строке
    if (coordY == rootRowRank) {
        MPI_Scatterv(
                (isRoot ? calculationSetup.matrixB->data : nullptr),
                columnsCount, firstColumns,
                columnType, verticalStrip.data, columnsCount[coordX],
                rowType, rootRowRank, rowCommunicator
        );
    }
    MPI_Bcast(horizontalStrip.data, linesCount[coordY], rowType, rootColRank, rowCommunicator);
    MPI_Bcast(verticalStrip.data, columnsCount[coordX], rowType, rootRowRank, columnCommunicator);

    auto result = horizontalStrip * verticalStrip;
    if (debug) {
        std::cout << "rank = " << mpiRank << "\n---\n";
        result.printMatrix();
        std::cout << "---\n";
    }

    delete[] firstLines;
    delete[] linesCount;
    delete[] firstColumns;
    delete[] columnsCount;
}
