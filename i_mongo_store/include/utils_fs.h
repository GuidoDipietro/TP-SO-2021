#ifndef UTILS_FS_H
#define UTILS_FS_H

#include <string.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#include "init_ims.h"

typedef struct {
    uint32_t block_size;
    uint32_t blocks;
    t_bitarray* bitarray;
    uint32_t bytes_bitarray;
    uint64_t tamanio_fs;
} t_superbloque;

//#define raw_bitmap_t char*
typedef char* raw_bitmap_t;

extern t_log* logger;

extern t_superbloque* superbloque;
extern uint8_t* mem_map;
extern uint8_t* mem_cpy;

extern pthread_t HILO_SINCRONIZADOR;

extern bool saboteado;
extern sem_t sem_sabotaje;
extern sem_t sem_inicio_fsck;

char* concatenar_montaje(char* str);
char* concatenar_montaje_files(char* filename);
char* concatenar_montaje_bitacora(char* filename);
char* path_bitacora(); // Devuelve el path de los archivos de bitacoras
char* path_files();

#endif