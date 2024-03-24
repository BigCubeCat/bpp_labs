#include "CalculationController.h"
#include "../utils/utils.h"

int countOfLines(size_t n, int rank, int size) {
    return (int) (n / (size_t) size) + ((rank < n ? 1 : 0) % size);
}

CalculationController::CalculationController(const TConfigStruct &tConfigStruct, int rank, int size)
        : mpiWorldRank(rank), mpiWorldSize(size) {
    n = tConfigStruct.matrixA->height;
    k = tConfigStruct.matrixA->width;
    m = tConfigStruct.matrixB->width;

    firstMatN = n * k;
    secondMatN = m * k;

    // Этот конструктор может вызываться только в 0 процессе
    auto pair = getOptimalGeometry(mpiWorldSize);
    countHorizontalStrips = pair.first;
    countVerticalStrips = pair.second;

    calcRowStripLines();
    calcColStripLines();
}

void CalculationController::calcRowStripLines() {
    linesCount.resize(countHorizontalStrips);
    firstLines[0] = 0;
    linesCount[0] = countOfLines(firstMatN, 0, mpiWorldSize);
    for (int i = 1; i < mpiWorldSize; ++i) {
        linesCount[i] = countOfLines(firstMatN, i, mpiWorldSize);
        firstLines[i] = firstLines[i - 1] + linesCount[i - 1];
    }
}

void CalculationController::calcColStripLines() {
    columnsCount.resize(countVerticalStrips);
}

CalculationController::CalculationController(int rank, int size)
        : mpiWorldRank(rank), mpiWorldSize(size) {
    calcRowStripLines();
    calcColStripLines();
}

CalculationController::~CalculationController() = default;
