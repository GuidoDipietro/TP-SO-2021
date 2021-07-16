#ifndef BITACORA_H
#define BITACORA_H

#include <commons/collections/list.h>
#include <stdint.h>
#include <stdio.h>

#include "utils_fs.h"

typedef struct {
    char* nombre;
    uint32_t block_count;
    uint32_t size;
    t_list* blocks;
} bitacora_t;

void escribir_bitacora(bitacora_t* bitacora);
void crear_bitacora(char* nombre);
void write_to_bitacora(bitacora_t* file, void* content, uint32_t len);
void cerrar_bitacora(bitacora_t* bitacora);
void* recuperar_bitacora(bitacora_t* file);
bitacora_t* cargar_bitacora(char* nombre);

#endif