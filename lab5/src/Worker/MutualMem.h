#include <pthread.h>
#include "EStatus.h"


#ifndef LAB5_MUTUALMEM_H
#define LAB5_MUTUALMEM_H

struct MutualMem {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    EStatus flag = RUNNING;
};

#endif
