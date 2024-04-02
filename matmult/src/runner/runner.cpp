#include <mpi.h>

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

void RunMultiplication(
        const std::string &input, const std::string &output,
        int mpiRank, int mpiSize, bool debug
) {
    MPI_Comm comm2d, rowCommunicator, columnCommunicator;
    int rootRowRank, rootColRank, coordX, coordY, n, k, m, dims[2] = {0, 0};
    bool isRoot = mpiRank == 0;
    setupComm(comm2d, rowCommunicator, columnCommunicator, coordX, coordY, rootColRank, rootRowRank, dims, mpiSize);

    TConfigStruct calculationSetup;
    if (isRoot) {
        calculationSetup = readFile(input, n, m, k);
        if (debug) calculationSetup.print();
    }
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
    if (mpiRank == 3) {
        horizontalStrip.printMatrix();
        verticalStrip.printMatrix();
        result.printMatrix();
    }

    // приходит тоже вроде верно
    if (isRoot) {
        MatrixModel resultMatrix = MatrixModel(n, k);
        resultMatrix.copy(result);
        MPI_Status status;
        int senderCoords[2] = {0, 0};
        for (int slaveId = 1; slaveId < mpiSize; ++slaveId) {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm2d, &status);
            MPI_Cart_coords(comm2d, status.MPI_SOURCE, 2, senderCoords);
            int typeIndex;
            auto startCoords = findStartCoord(senderCoords, dims, normalSizeX, normalSizeY, n, k, typeIndex);
            MPI_Recv(
                    resultMatrix.data + startCoords.first * k + startCoords.second,
                    1, cells[typeIndex], status.MPI_SOURCE, 0, comm2d, &status
            );
        }
        FileWorker(output, false).writeMat(resultMatrix);
    } else {
        MPI_Send(result.data, static_cast<int>(result.dataSize), MPI_DOUBLE, 0, 0, comm2d);
    }
    delete[] firstLines;
    delete[] linesCount;
    delete[] firstColumns;
    delete[] columnsCount;
}
