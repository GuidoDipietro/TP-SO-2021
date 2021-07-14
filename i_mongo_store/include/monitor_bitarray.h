#ifndef MONITOR_BITARRAY_H
#define MONITOR_BITARRAY_H

#include <pthread.h>

#include "utils_fs.h"

extern pthread_mutex_t MUTEX_BITARRAY;

void monitor_bitarray_set_bit(uint64_t offset);
void monitor_bitarray_clean_bit(uint64_t offset);
bool monitor_test_bit(uint64_t offset);
uint32_t monitor_offset_bloque_libre();

#endif