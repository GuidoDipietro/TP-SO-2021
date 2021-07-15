#ifndef FILES_H
#define FILES_H

#include <stdint.h>
#include <commons/collections/list.h>
#include <pthread.h>

#include "monitor_lista_archivos.h"
#include "utils_fs.h"

void escribir_archivo(char* nombre, file_t* file);
void crear_archivo(char* nombre, char c);
file_t* cargar_archivo(char* nombre);

#endif