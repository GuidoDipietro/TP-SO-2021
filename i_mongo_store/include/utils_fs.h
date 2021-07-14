#ifndef UTILS_FS_H
#define UTILS_FS_H

#include <string.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <stdint.h>

#include "init_ims.h"

typedef struct {
    uint32_t block_size;
    uint32_t blocks;
    t_bitarray* bitarray;
    uint32_t bytes_bitarray;
    uint64_t tamanio_fs;
} superbloque_t;

//#define raw_bitmap_t char*
typedef char* raw_bitmap_t;

extern t_log* logger;

extern superbloque_t superbloque;
extern uint8_t* mem_map;
extern uint8_t* mem_cpy;

char* concatenar_montaje(char* str);

#endif