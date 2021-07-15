#ifndef BLOCKS_H
#define BLOCKS_H

#include <pthread.h>

#include "utils_fs.h"
#include "monitor_bitarray.h"

extern pthread_mutex_t MUTEX_BLOCKS;

void sincronizador();
void escribir_bloque(void* content, uint32_t nro_bloque, uint64_t size);
void* leer_bloque(uint32_t nro_bloque);
void liberar_bloque(uint32_t nro_bloque);
void append_to_block(void* content, uint32_t nro_bloque, uint64_t offset, uint64_t size);

#endif