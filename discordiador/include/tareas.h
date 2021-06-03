#ifndef TAREAS_H
#define TAREAS_H

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include "ops_tripulante.h"
#include "monitor_tripulante.h"
#include "config.h"
#include "../../shared/include/protocolo.h"

bool PLANIFICADOR_ALIVE;

extern void* correr_tripulante;

void planificador();
void correr_tripulante_FIFO(t_running_thread* thread_data);
void correr_tripulante_RR(t_running_thread* thread_data);
uint8_t replanificar_tripulante(t_running_thread* thread_data, t_tripulante* t);
void correr_tarea(t_running_thread* r_t);
void desalojar_tripulante(t_running_thread* thread_data);

#endif
