#include "./setup.h"

void setupComm(
        MPI_Comm &comm2d, MPI_Comm &rowCommunicator, MPI_Comm &columnCommunicator,
        int &coordX, int &coordY, int &rootColRank, int &rootRowRank, int *dims,
        int mpiSize) {
    int rankComm2D;
    int periods[2] = {0, 0}, coords[2];
    if (dims[0] == 0 && dims[1] == 0)
        MPI_Dims_create(mpiSize, 2, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm2d);
    MPI_Comm_rank(comm2d, &rankComm2D);
    MPI_Cart_get(comm2d, 2, dims, periods, coords);
    coordY = coords[0];
    coordX = coords[1];
    MPI_Cart_coords(comm2d, 0, 2, coords);
    rootRowRank = coords[0];
    rootColRank = coords[1];
    MPI_Comm_split(comm2d, coordY, coordX, &rowCommunicator);
    MPI_Comm_split(comm2d, coordX, coordY, &columnCommunicator);
}

void setupDatatypes(
        MPI_Datatype *rowType, MPI_Datatype *columnType,
        MPI_Datatype *cells,
        const int *dims, int n, int m, int k) {
    //row
    MPI_Type_contiguous(m, MPI_DOUBLE, rowType);
    MPI_Type_commit(rowType);
    //column
    MPI_Type_vector(m, 1, k, MPI_DOUBLE, columnType);
    MPI_Type_create_resized(*columnType, 0, sizeof(double), columnType);
    MPI_Type_commit(columnType);

    MPI_Type_vector(n / dims[0], k / dims[1], k, MPI_DOUBLE, &cells[3]);
    MPI_Type_create_resized(cells[3], 0, k / dims[1] * sizeof(double), &cells[3]);
    MPI_Type_commit(&cells[3]);

    MPI_Type_vector(n / dims[0] + 1, k / dims[1], k, MPI_DOUBLE, &cells[2]);
    MPI_Type_create_resized(cells[2], 0, k / dims[1] * sizeof(double), &cells[2]);
    MPI_Type_commit(&cells[2]);

    MPI_Type_vector(n / dims[0], k / dims[1] + 1, k, MPI_DOUBLE, &cells[1]);
    MPI_Type_create_resized(cells[1], 0, (k / dims[1] + 1) * sizeof(double), &cells[1]);
    MPI_Type_commit(&cells[1]);

    MPI_Type_vector(n / dims[0] + 1, k / dims[1] + 1, k, MPI_DOUBLE, &cells[0]);
    MPI_Type_create_resized(cells[0], 0, (k / dims[1] + 1) * sizeof(double), &cells[0]);
    MPI_Type_commit(&cells[0]);
}

void setupLines(int *firstLines, int *linesCount, int *firstColumns, int *columnsCount, const int *dims, int n, int k) {
    firstLines[0] = 0;
    linesCount[0] = n / dims[0] + ((n % dims[0] > 0) ? 1 : 0);
    for (int i = 1; i < dims[0]; ++i) {
        linesCount[i] = n / dims[0] + ((n % dims[0] > 0 && i < n % dims[0]) ? 1 : 0);
        firstLines[i] = linesCount[i - 1] + firstLines[i - 1];
    }
    firstColumns[0] = 0;
    columnsCount[0] = k / dims[1] + ((k % dims[1] > 0) ? 1 : 0);
    for (int i = 1; i < dims[1]; ++i) {
        columnsCount[i] = k / dims[1] + ((k % dims[1] > 0 && i < k % dims[1]) ? 1 : 0);
        firstColumns[i] = columnsCount[i - 1] + firstColumns[i - 1];
    }
}

std::pair<int, int> findStartCoord(
        const int *senderCoords, const int *dims,
        int normalSizeX, int normalSizeY, int n, int k,
        int &typeIndex
) {
    std::pair<int, int> result;
    if (senderCoords[0] < normalSizeY) {
        if (senderCoords[1] < normalSizeX) {
            typeIndex = 0;
            result.first = senderCoords[0] * (n / dims[0] + 1);
            result.second = senderCoords[1] * (k / dims[1] + 1);
        } else {
            typeIndex = 2;
            result.first = senderCoords[0] * (n / dims[0] + 1);
            result.second = senderCoords[1] * (k / dims[1]) + normalSizeX;
        }
    } else {
        if (senderCoords[1] < normalSizeX) {
            typeIndex = 1;
            result.first = senderCoords[0] * (n / dims[0]) + normalSizeY;
            result.second = senderCoords[1] * (k / dims[1] + 1);
        } else {
            typeIndex = 3;
            result.first = senderCoords[0] * (n / dims[0]) + normalSizeY;
            result.second = senderCoords[1] * (k / dims[1]) + normalSizeX;
        }
    }
    return result;
}
