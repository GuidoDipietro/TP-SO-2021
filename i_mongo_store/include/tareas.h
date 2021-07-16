#ifndef TAREAS_H
#define TAREAS_H

#include <semaphore.h>

#include "files.h"
#include "controlador.h"
#include "bitacora.h"

void tarea_generar(tipo_item tipo, uint32_t cantidad);
void tarea_consumir(tipo_item tipo, uint32_t cantidad);
void descartar_basura();
void completar_bitacora(char* nombre, void* content, uint32_t len);
char* obtener_bitacora(char* nombre);

#endif