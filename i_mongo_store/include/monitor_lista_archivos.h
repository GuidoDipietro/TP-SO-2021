#ifndef MONITOR_LISTA_ARCHIVOS_H
#define MONITOR_LISTA_ARCHIVOS_H

#include <pthread.h>

#include "utils_fs.h"

typedef struct {
    uint32_t size;
    uint32_t block_count;
    t_list* blocks;
    char caracter_llenado;
    char* md5; // Siempre de 32 caracteres
} file_t;

typedef struct {
    char* nombre;
    file_t* file;
    pthread_mutex_t mutex_file;
    uint32_t refs;
} open_file_t;

extern t_list* OPEN_FILES;
extern pthread_mutex_t MUTEX_LISTA_ARCHIVOS;

void agregar_archivo(open_file_t* file_data);
open_file_t* obtener_archivo(char* nombre);
void monitor_iterar_lista_archivos(void (*f)(open_file_t*));
void remover_lista_archivos(char* nombre);

#endif