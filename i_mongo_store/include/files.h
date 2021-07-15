#ifndef FILES_H
#define FILES_H

#include <stdint.h>
#include <commons/collections/list.h>
#include <pthread.h>

#include "monitor_lista_archivos.h"
#include "utils_fs.h"

void escribir_archivo(char* nombre, file_t* file);
void crear_archivo(char* nombre, char c);
void print_file_t(file_t* file);
void print_open_file_t(open_file_t* file_data);
open_file_t* cargar_archivo(char* nombre);
void write_to_file(open_file_t* file_data, void* content, uint32_t len);
void* recuperar_archivo(open_file_t* file_data);
void guardar_metadata_todos();
void guardar_metadata_todos_ignorar_mutex();
void generar_recurso(open_file_t* file_data, uint32_t cantidad);
void cerrar_archivo(open_file_t* file_data);
void eliminar_archivo(open_file_t* file_data);

#endif