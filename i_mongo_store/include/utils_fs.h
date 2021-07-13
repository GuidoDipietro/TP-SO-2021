#ifndef UTILS_FS_H
#define UTILS_FS_H

#include <string.h>
#include <commons/bitarray.h>

#include "init_ims.h"

typedef struct {
    uint32_t block_size;
    uint32_t blocks;
    t_bitarray* bitarray;
    uint32_t bytes_bitarray;
} superbloque_t;

//#define raw_bitmap_t char*
typedef char* raw_bitmap_t;

extern superbloque_t superbloque;
extern void* mem_map;
extern void* mem_cpy;

char* concatenar_montaje(char* str);

#endif