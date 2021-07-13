#ifndef MONITOR_BITARRAY_H
#define MONITOR_BITARRAY_H

#include <pthread.h>

#include "utils_fs.h"

extern pthread_mutex_t MUTEX_BITARRAY;

void monitor_bitarray_set_bit(t_bitarray* t, uint64_t offset);
void monitor_bitarray_clean_bit(t_bitarray* t, uint64_t offset);
bool monitor_test_bit(t_bitarray* t, uint64_t offset);

#endif