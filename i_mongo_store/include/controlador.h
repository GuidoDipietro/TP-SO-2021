#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include <semaphore.h>
#include <pthread.h>
#include <commons/collections/queue.h>

#include "utils_fs.h"

extern sem_t OPERACIONES_PENDIENTES;
extern sem_t DISCO_LIBRE;
extern pthread_mutex_t MUTEX_COLA_OPERACIONES;

void controlador_disco();
void agregar_controlador_disco(sem_t* semaphore);
sem_t* remover_controlador_disco();

#endif