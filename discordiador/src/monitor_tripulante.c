#include "../include/monitor_tripulante.h"

pthread_mutex_t MUTEX_COLA;

void iniciar_mutex() {
    pthread_mutex_init(&MUTEX_COLA, NULL);
}

void finalizar_mutex() {
    pthread_mutex_destroy(&MUTEX_COLA);
}

void push_cola_tripulante(t_tripulante* t) {
    pthread_mutex_lock(&MUTEX_COLA);
    queue_push(COLA_TRIPULANTES, (void*) t);
    pthread_mutex_unlock(&MUTEX_COLA);
}

void pop_cola_tripulante() {
    pthread_mutex_lock(&MUTEX_COLA);
    void* t = queue_pop(COLA_TRIPULANTES);
    pthread_mutex_unlock(&MUTEX_COLA);
    return (t_tripulante*) t;
}