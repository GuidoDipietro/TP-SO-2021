#ifndef BLOCKS_H
#define BLOCKS_H

#include <pthread.h>
#include <semaphore.h>

#include "utils_fs.h"
#include "monitor_bitarray.h"


void sincronizador();
void sincronizar_fs_ignorar_mutex();
void sincronizar_fs();
void escribir_bloque(void* content, uint32_t nro_bloque, uint64_t size);
void* leer_bloque(uint32_t nro_bloque);
void liberar_bloque(uint32_t nro_bloque);
void append_to_block(void* content, uint32_t nro_bloque, uint64_t offset, uint64_t size);
void quitar_de_bloque(uint32_t nro_bloque, uint32_t size_bloque,  uint32_t cantidad);
uint32_t size_bloque(uint32_t nro_bloque, char c);
void sincronizar_datos_superbloque();
void sincronizar_bitarray();

extern pthread_mutex_t MUTEX_BLOCKS;
extern sem_t sem_sabotaje_sincronizador;

#endif