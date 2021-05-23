#include "../include/monitor_tripulante.h"

// Funcion y variable para buscar_cola_tripulante()

t_list* LISTA_HILOS;
static uint16_t obj_tid = 0;

bool filter_by_tid(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;
    return t->tid == obj_tid;
}

// Funciones publicas

pthread_mutex_t MUTEX_COLA;
pthread_mutex_t MUTEX_LISTA_HILOS;

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
    pthread_mutex_init(&MUTEX_LISTA_HILOS, NULL);
}

void finalizar_mutex() {
    pthread_mutex_destroy(&MUTEX_COLA);
    pthread_mutex_destroy(&MUTEX_LISTA_HILOS);
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

uint16_t largo_cola() {
    pthread_mutex_lock(&MUTEX_COLA);
    uint16_t ret = queue_size(COLA_TRIPULANTES);
    pthread_mutex_unlock(&MUTEX_COLA);
    return ret;
}

void remover_cola_tripulante(uint16_t tid) {
    pthread_mutex_lock(&MUTEX_COLA);
    list_remove_and_destroy_by_condition(COLA_TRIPULANTES->elements, filter_by_tid, free_t_tripulante);
    pthread_mutex_unlock(&MUTEX_COLA);
}

void iterar_cola(void (*function)(void*)) {
    pthread_mutex_lock(&MUTEX_COLA);
    list_iterate(COLA_TRIPULANTES->elements, function);
    pthread_mutex_unlock(&MUTEX_COLA);
}

//
// Funciones de LISTA_HILOS
//

void monitor_add_lista_hilos(void* t) {
    pthread_mutex_lock(&MUTEX_LISTA_HILOS);
    list_add(LISTA_HILOS, t);
    pthread_mutex_unlock(&MUTEX_LISTA_HILOS);
}

void* monitor_remove_by_condition_lista_hilos(bool (*f)(void*)) {
    pthread_mutex_lock(&MUTEX_LISTA_HILOS);
    void* ret = list_remove_by_condition(LISTA_HILOS, f);
    pthread_mutex_unlock(&MUTEX_LISTA_HILOS);
    return ret;
}