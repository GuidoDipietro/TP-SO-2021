#ifndef TAREAS_H
#define TAREAS_H

#include "files.h"

void tarea_generar(char* nombre, char c, uint32_t cantidad);
void tarea_consumir(char* nombre, uint32_t cantidad);
void descartar_basura();

#endif