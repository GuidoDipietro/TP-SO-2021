#ifndef TAREAS_H
#define TAREAS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <commons/collections/list.h>

#include "ciclo.h"
#include "ops_tripulante.h"
#include "monitor_tripulante.h"
#include "config.h"
#include "entrada_salida.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/utils.h"

bool PLANIFICADOR_ALIVE;

extern bool SABOTAJE_ACTIVO;

extern void* correr_tripulante;

void planificador();
void correr_tripulante_FIFO(t_running_thread* thread_data);
void correr_tripulante_RR(t_running_thread* thread_data);
uint8_t replanificar_tripulante(t_running_thread* thread_data, t_tripulante* t);
void mover_tripulante(t_running_thread* r_t);
void correr_tarea_generica(t_running_thread* r_t);
void desalojar_tripulante(t_running_thread* thread_data, t_tripulante* t);
bool posiciones_iguales(t_posicion* p1, t_posicion* p2);

#endif
