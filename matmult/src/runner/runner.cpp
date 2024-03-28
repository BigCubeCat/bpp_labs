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

    MPI_Datatype rowStrip, columnStrip;
    MPI_Type_contiguous(m, MPI_DOUBLE, &rowStrip);
    MPI_Type_vector(n, 1, k - 1, MPI_DOUBLE, &columnStrip);
    MPI_Type_commit(&rowStrip);
    MPI_Type_commit(&columnStrip);

    // TODO: вынести в отдельную функцию следующий код
    int *linesCount = new int[dims[0]];
    int *firstLines = new int[dims[0]];
    int *columnsCount = new int[dims[1]];
    int *firstColumns = new int[dims[1]];
    firstLines[0] = 0;
    linesCount[0] = n / dims[0] + ((n % dims[0] > 0) ? 1 : 0);
    for (int i = 1; i < dims[0]; ++i) {
        linesCount[i] = n / dims[0];
        if (n % dims[0] > 0 && i < n % dims[0]) {
            linesCount[i]++;
        }
        firstLines[i] = linesCount[i - 1] + firstLines[i - 1];
    }
    firstColumns[0] = 0;
    columnsCount[0] = k / dims[1] + ((k % dims[1] > 0) ? 1 : 0);
    for (int i = 1; i < dims[1]; ++i) {
        columnsCount[i] = k / dims[1];
        if (k % dims[1] > 0 && i < k % dims[1]) {
            columnsCount[i]++;
        }
        firstColumns[i] = firstLine(k, i, dims[1]);
    }
    // end TODO

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

    std::cout << "linesCount = " << linesCount[coordY] << " " << m << std::endl;
    MatrixModel horizontalStrip = MatrixModel(linesCount[coordY], m);
    MatrixModel verticalStrip = MatrixModel(m, columnsCount[coordX]);

    if (mpiRank == 0) {
        horizontalStrip.data = calculationSetup.matrixA->data;
        for (int i = 1; i < dims[0]; ++i) {
            MPI_Send(calculationSetup.matrixA->data + firstLines[i] * m, linesCount[i], rowStrip, i, 0,
                     columnCommunicator);
        }

    } else if (coordX == 0) {
        MPI_Recv(horizontalStrip.data, linesCount[coordY], rowStrip, 0, 0, columnCommunicator, MPI_STATUS_IGNORE);
    }
    MPI_Bcast(horizontalStrip.data, linesCount[coordY], rowStrip, rootRowRank, rowCommunicator);
    //MPI_Bcast(verticalStrip.data, columnsCount[coordX], columnStrip, rootColRank, columnCommunicator);

    std::cout << mpiRank << "horizontal strip = ";
    horizontalStrip.printMatrix();

    std::cout << mpiRank << "vertical strip = ";
    verticalStrip.printMatrix();

    MPI_Type_free(&rowStrip);
    MPI_Type_free(&columnStrip);

    delete[] linesCount;
    delete[] firstLines;
}
