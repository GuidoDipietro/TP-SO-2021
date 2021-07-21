#include "../include/controlador.h"

t_queue* COLA_OPERACIONES;
sem_t OPERACIONES_PENDIENTES;
sem_t DISCO_LIBRE;
pthread_mutex_t MUTEX_COLA_OPERACIONES;

void controlador_disco() {
    log_info(logger, "Controlador de disco iniciado.");
    COLA_OPERACIONES = queue_create();
    while(1) {
        sem_wait(&OPERACIONES_PENDIENTES);

        if(saboteado)
            sem_wait(&sem_sabotaje);

        sem_t* sem = remover_controlador_disco();
        sem_post(sem);
        sem_wait(&DISCO_LIBRE);
    }
}

void agregar_controlador_disco(sem_t* semaphore) {
    pthread_mutex_lock(&MUTEX_COLA_OPERACIONES);
    queue_push(COLA_OPERACIONES, semaphore);
    sem_post(&OPERACIONES_PENDIENTES);
    pthread_mutex_unlock(&MUTEX_COLA_OPERACIONES);
}

sem_t* remover_controlador_disco() {
    pthread_mutex_lock(&MUTEX_COLA_OPERACIONES);
    sem_t* ret = queue_pop(COLA_OPERACIONES);
    pthread_mutex_unlock(&MUTEX_COLA_OPERACIONES);
    return ret;
}