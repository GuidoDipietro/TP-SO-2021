#ifndef TAREAS_H
#define TAREAS_H

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include "ops_tripulante.h"
#include "config.h"

typedef struct {
    t_tripulante* t;
    pthread_t thread;
} t_running_thread;

pthread_t PLANIFICADOR;
sem_t active_threads;
t_list* LISTA_HILOS;

void planificador();
void correr_tarea_FIFO(t_tripulante*);
void reasignar_tripulante(t_tripulante*);

#endif
