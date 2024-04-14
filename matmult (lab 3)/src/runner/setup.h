#ifndef MATMULT_SETUP_H
#define MATMULT_SETUP_H

#include <mpi.h>
#include <utility>

/*
 * Создание сети комуникаторов.
 */
void setupComm(
        MPI_Comm &comm2d, MPI_Comm &rowCommunicator, MPI_Comm &columnCommunicator,
        int &coordX, int &coordY, int &rootColRank, int &rootRowRank, int *dims,
        int mpiSize
);

/*
 * Настройка типов
 * rowType, columnType - строка и столбец
 * Остальное - клетки. Каждая клетка либо чуть больше либо чуть уже
 * cells[0] - большая по X и по Y
 * cells[1] - большая по Y, малая по X
 * cells[2] - большая по X, малая по Y
 * cells[3] - малая
 */
void setupDatatypes(
        MPI_Datatype *rowType, MPI_Datatype *columnType,
        MPI_Datatype *cels,
        const int *dims, int n, int m, int k
);

void setupLines(int *firstLines, int *linesCount, int *firstColumns, int *columnsCount, const int *dims, int n, int k);

std::pair<int, int> findStartCoord(
        const int *senderCoords, const int *dims,
        int normalSizeX, int normalSizeY, int n, int k,
        int &typeIndex
);

#endif
