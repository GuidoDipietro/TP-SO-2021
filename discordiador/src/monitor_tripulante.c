#include "../include/monitor_tripulante.h"

// Funciones estaticas

// Funcion y variable para buscar_cola_tripulante()

static uint16_t obj_tid = 0;

bool filter_by_tid(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;
    return t->tid == obj_tid;
}

// Funciones publicas

pthread_mutex_t MUTEX_COLA;

void free_t_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;

    if(t->pos != NULL)
        free_t_posicion(t->pos);

    if(t->tarea != NULL)
        free_t_tarea(t->tarea);

    free(t);
}

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
    t->status = READY;
}

t_tripulante* pop_cola_tripulante() {
    pthread_mutex_lock(&MUTEX_COLA);
    void* t = queue_pop(COLA_TRIPULANTES);
    pthread_mutex_unlock(&MUTEX_COLA);
    return (t_tripulante*) t;
}

t_tripulante* buscar_cola_tripulante(uint16_t tid) {
    pthread_mutex_lock(&MUTEX_COLA);
    obj_tid = tid;
    void* p = list_find(COLA_TRIPULANTES->elements, filter_by_tid);
    pthread_mutex_unlock(&MUTEX_COLA);
    return p;
}

void remover_cola_tripulante(uint16_t tid) {
    pthread_mutex_lock(&MUTEX_COLA);
    list_remove_and_destroy_by_condition(COLA_TRIPULANTES->elements, filter_by_tid, free_t_tripulante);
    pthread_mutex_unlock(&MUTEX_COLA);
}