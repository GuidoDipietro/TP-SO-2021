#include "../include/monitor_bitarray.h"

pthread_mutex_t MUTEX_BITARRAY;

void monitor_bitarray_set_bit(t_bitarray* t, uint64_t offset) {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    bitarray_set_bit(t, offset);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
}

void monitor_bitarray_clean_bit(t_bitarray* t, uint64_t offset) {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    bitarray_clean_bit(t, offset);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
}

bool monitor_test_bit(t_bitarray* t, uint64_t offset) {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    bool ret = bitarray_test_bit(t, offset);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
    return ret;
}