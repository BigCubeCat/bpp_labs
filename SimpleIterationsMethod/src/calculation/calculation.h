#ifndef CALCULATION_H
#define CALCULATION_H

#include "../CtxData/CtxData.h"

/*
 * Количество линий в частиче матрицы
 */
int countOfLines(int n, int rank, int size);

/*
 * Конфигурация данных по процессам
 */
void initLinesSettings(int *linesCount, int *firstLines, int size, int n);

/* Считаем конфигурацию себя и других */
void prepare(CtxData *data, int size);

void syncStartData(CtxData *data, int rank, int size);

#endif
