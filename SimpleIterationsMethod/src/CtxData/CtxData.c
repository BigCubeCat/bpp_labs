#include <stdlib.h>
#include "./CtxData.h"

void bLenCalc(CtxData *data, double epsilon) {
    data->b_length = 0;
    for (int i = 0; i < data->n; ++i) {
        data->b_length += data->b_vector[i] * data->b_vector[i];
    }
    data->b_length *= epsilon;
    data->b_length *= epsilon;
}

void freeCtx(CtxData *data) {
    free(data->matrix);
    free(data->x_vector);
    free(data->x_new_vector);
    free(data->b_vector);
    free(data->linesCount);
    free(data->firstLines);
}
