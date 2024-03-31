#ifndef MATMULT_SETUP_H
#define MATMULT_SETUP_H

#include <mpi.h>

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
 * wide\narrow - является ли колонка широкой или узкой
 * Long\Short - является ли полоска выше или ниже
 */
void setupDatatypes(
        MPI_Datatype *rowType, MPI_Datatype *columnType,
        MPI_Datatype *wideLongCell, MPI_Datatype *wideShortCell,
        MPI_Datatype *narrowLongCell, MPI_Datatype *narrowShortCell,
        const int *dims, int n, int m, int k
);

void setupLines(int *firstLines, int *linesCount, int *firstColumns, int *columnsCount, const int *dims, int n, int k);

#endif
