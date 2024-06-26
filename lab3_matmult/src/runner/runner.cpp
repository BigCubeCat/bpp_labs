#include <mpi.h>
#include <iostream>

#include "./runner.h"
#include "../FileWorker/FileWorker.h"
#include "setup.h"

TConfigStruct readFile(const std::string &filename, int &n, int &m, int &k) {
    FileWorker fw = FileWorker(filename, true);
    TConfigStruct calculationSetup = fw.readData();
    n = calculationSetup.n;
    m = calculationSetup.m;
    k = calculationSetup.k;
    return calculationSetup;
}

double RunMultiplication(const std::string &input, const std::string &output, int *dims, bool debug) {
    int mpiRank, mpiSize;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    MPI_Comm comm2d, rowCommunicator, columnCommunicator;
    int rootRowRank, rootColRank, coordX, coordY, n, k, m;
    bool isRoot = mpiRank == 0;
    setupComm(comm2d, rowCommunicator, columnCommunicator, coordX, coordY, rootColRank, rootRowRank, dims, mpiSize);
    if (debug) {
        std::cout << "dims = " << dims[0] << " " << dims[1] << std::endl;
    }

    TConfigStruct calculationSetup;
    if (isRoot) {
        calculationSetup = readFile(input, n, m, k);
        if (debug) calculationSetup.print();
    }
    double startTime = MPI_Wtime(); // замеряем время ПОСЛЕ чтения из данных,
    // так как мы тут не файловую систему тестируем
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int normalSizeX = k % dims[1];
    int normalSizeY = n % dims[0];

    int *firstLines = new int[dims[0]];
    int *linesCount = new int[dims[0]];
    int *firstColumns = new int[dims[1]];
    int *columnsCount = new int[dims[1]];
    setupLines(firstLines, linesCount, firstColumns, columnsCount, dims, n, k);

    MPI_Datatype rowType, columnType;
    MPI_Datatype cells[4];
    setupDatatypes(&rowType, &columnType, cells, dims, n, m, k);

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

    auto *resultMatrix = (mpiRank == 0) ? new MatrixModel(n, k) : nullptr;
    // приходит тоже вроде верно
    if (isRoot) {
        resultMatrix->copy(result);
        MPI_Status status;
        int senderCoords[2] = {0, 0};
        for (int slaveId = 1; slaveId < mpiSize; ++slaveId) {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm2d, &status);
            MPI_Cart_coords(comm2d, status.MPI_SOURCE, 2, senderCoords);
            int typeIndex;
            auto startCoords = findStartCoord(senderCoords, dims, normalSizeX, normalSizeY, n, k, typeIndex);
            MPI_Recv(
                    resultMatrix->data + startCoords.first * k + startCoords.second,
                    1, cells[typeIndex], status.MPI_SOURCE, 0, comm2d, &status
            );
        }
    } else {
        MPI_Send(result.data, static_cast<int>(result.dataSize), MPI_DOUBLE, 0, 0, comm2d);
    }
    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;
    double maxTime;
    MPI_Reduce(&elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);
    MPI_Finalize();
    if (mpiRank == 0) {
        FileWorker(output, false).writeMat(*resultMatrix);
    }
    delete resultMatrix;
    delete[] firstLines;
    delete[] linesCount;
    delete[] firstColumns;
    delete[] columnsCount;
    return (mpiRank == 0) ? maxTime : 0;
}
