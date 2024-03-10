#ifndef CTXDATA_H
#define CTXDATA_H

#include <stdio.h>

typedef struct data {
    size_t n;
    double *matrix;
    double *b_vector;
    double *x_vector;
    double *x_new_vector;
    int *linesCount;
    int *firstLines;
    double b_length;
} CtxData;

void bLenCalc(CtxData *data, double epsilon);

void freeCtx(CtxData *data);

#endif
