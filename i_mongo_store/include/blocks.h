#ifndef BLOCKS_H
#define BLOCKS_H

#include <pthread.h>

#include "utils_fs.h"
#include "monitor_bitarray.h"

extern pthread_mutex_t MUTEX_BLOCKS;

void sincronizador();

#endif