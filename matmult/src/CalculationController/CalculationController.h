#ifndef MATMULT_CALCULATIONCONTROLLER_H
#define MATMULT_CALCULATIONCONTROLLER_H


#include <cstdio>
#include <vector>
#include "../FileWorker/FileWorker.h"

struct CalculationController {
    int mpiWorldRank{}, mpiWorldSize{}; // MPI rank and MPI size

    size_t n{}, k{}, m{}; // размеры матрицы
    size_t firstMatN{};
    size_t secondMatN{};

    int countVerticalStrips = 0; // число вертикальных полос
    int countHorizontalStrips = 0; // число горизонтальных полос

    std::vector<int> linesCount; // Число строк в каждой полосе
    std::vector<int> firstLines; // индекс начала строк
    std::vector<int> columnsCount; // Число колонок в каждой колонне

    int sizeOfVerticalStrip = 0;
    int sizeOfHorizontalStrip = 0;

    void calcRowStripLines();

    void calcColStripLines();

    explicit CalculationController(const TConfigStruct &tConfigStruct, int rank, int size);
    explicit CalculationController(int rank, int size);
    ~CalculationController();
};


#endif
