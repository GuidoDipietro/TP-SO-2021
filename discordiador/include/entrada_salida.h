#ifndef ENTRADA_SALIDA_H
#define ENTRADA_SALIDA_H

#include "monitor_tripulante.h"
#include "ciclo.h"
#include "sabotajes.h"

void controlador_es();
void tarea_io(t_running_thread* thread, t_tripulante* t);

//extern sem_t pausa_io;

#endif