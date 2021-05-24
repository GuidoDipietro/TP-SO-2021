#ifndef TAREAS_H
#define TAREAS_H

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include "ops_tripulante.h"
#include "monitor_tripulante.h"
#include "config.h"

sem_t active_threads;

void planificador();
void correr_tarea_FIFO(t_running_thread*);
void reasignar_tripulante(t_tripulante*);

#endif
