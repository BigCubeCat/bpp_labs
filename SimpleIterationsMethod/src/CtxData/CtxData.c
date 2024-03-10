#include <stdlib.h>
#include <omp.h>
#include "./CtxData.h"

void bLenCalc(CtxData *data, double epsilon) {
    double res = 0;
#pragma omp parallel for reduction(+ : res)
    for (int i = 0; i < data->n; ++i) {
        res +=  data->b_vector[i] * data->b_vector[i];
    }
    data->b_length = res * epsilon * epsilon;
}

void freeCtx(CtxData *data) {
    free(data->matrix);
    free(data->x_vector);
    free(data->x_new_vector);
    free(data->b_vector);
    free(data->linesCount);
    free(data->firstLines);
}
