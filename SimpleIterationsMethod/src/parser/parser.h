#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "../CtxData/CtxData.h"

int readData(CtxData *data, char* filename);

void generateData(CtxData *data, size_t n);

#endif
