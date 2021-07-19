#include "../include/monitor_bitarray.h"

pthread_mutex_t MUTEX_BITARRAY;

void monitor_bitarray_set_bit(uint64_t offset) {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    bitarray_set_bit(superbloque->bitarray, offset);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
    //log_info(logger, "Set bit - Bloque %ld", offset);
}

void monitor_bitarray_clean_bit(uint64_t offset) {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    bitarray_clean_bit(superbloque->bitarray, offset);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
    //log_info(logger, "Clean bit - Block %ld", offset);
}

bool monitor_test_bit(uint64_t offset) {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    bool ret = bitarray_test_bit(superbloque->bitarray, offset);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
    return ret;
}

uint32_t monitor_offset_bloque_libre() {
    uint32_t offset = 0;
    for(uint32_t i = 0; i < superbloque->blocks; i++) {
        if(!bitarray_test_bit(superbloque->bitarray, offset))
            return offset;
        offset++;
    }
}