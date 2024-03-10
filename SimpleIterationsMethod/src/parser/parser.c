#include "./parser.h"
#include <stdio.h>
#include <malloc.h>

int readData(CtxData *data, char *filename) {
    FILE *file;

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    fscanf(file, "%zd", &data->n);
    data->matrix = (double *) malloc(sizeof(double) * data->n * data->n);
    data->b_vector = (double *) malloc(sizeof(double) * data->n);
    data->x_vector = (double *) malloc(sizeof(double) * data->n);
    data->x_new_vector = (double *) calloc(data->linesCount[0], sizeof(double));

    for (int i = 0; i < data->n; i++) {
        for (int j = 0; j < data->n; j++) {
            fscanf(file, "%lf", &data->matrix[i * data->n + j]);
        }
    }
    for (int i = 0; i < data->n; i++) {
        fscanf(file, "%lf", &data->b_vector[i]);
    }
    fclose(file);
    return 0;
}

void generateData(CtxData *data, size_t n) {
    data->n = n;

    data->matrix = (double *) malloc(sizeof(double) * data->n * data->n);
    data->b_vector = (double *) malloc(sizeof(double) * data->n);
    data->x_vector = (double *) malloc(sizeof(double) * data->n);
    data->x_new_vector = (double *) calloc(data->linesCount[0], sizeof(double));

    for (int i = 0; i < data->n; ++i) {
        data->x_vector[i] = 0;
        data->b_vector[i] = (double) n + 1;
        for (int j = 0; j < n; ++j) {
            data->matrix[i * n + j] = 1 + (i == j);
        }
    }
}
