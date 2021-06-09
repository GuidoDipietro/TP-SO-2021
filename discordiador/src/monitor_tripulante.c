#include "../include/monitor_tripulante.h"

// Funcion y variable para buscar_cola_tripulante()

static uint32_t obj_tid = 0;

bool filter_by_tid(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;
    return t->tid == obj_tid;
}

bool filter_t_running_thread_by_tid(void* item) {
    t_running_thread* t_r = (t_running_thread*) item;
    return t_r->t->tid == obj_tid;
}

// Funciones publicas

pthread_mutex_t MUTEX_COLA;
pthread_mutex_t MUTEX_LISTA_HILOS;
pthread_mutex_t MUTEX_LISTA_NEW;
pthread_mutex_t MUTEX_COLA_EXIT;

void free_t_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;

    if(t->pos != NULL) free_t_posicion(t->pos);
    if (t->tarea != NULL) free_t_tarea(t->tarea);

    free(t);
}

void iniciar_mutex() {
    pthread_mutex_init(&MUTEX_COLA, NULL);
    pthread_mutex_init(&MUTEX_LISTA_HILOS, NULL);
    pthread_mutex_init(&MUTEX_LISTA_NEW, NULL);
    pthread_mutex_init(&MUTEX_COLA_EXIT, NULL);
    sem_init(&TRIPULANTES_EN_COLA, 0, 0);
}

void finalizar_mutex() {
    pthread_mutex_destroy(&MUTEX_COLA);
    pthread_mutex_destroy(&MUTEX_LISTA_HILOS);
    pthread_mutex_destroy(&MUTEX_LISTA_NEW);
    pthread_mutex_destroy(&MUTEX_COLA_EXIT);
    sem_destroy(&TRIPULANTES_EN_COLA);
}

// Lista new

void push_cola_new(t_running_thread* n) {
    pthread_mutex_lock(&MUTEX_LISTA_NEW);
    queue_push(COLA_NEW, n);
    pthread_mutex_unlock(&MUTEX_LISTA_NEW);
}

t_running_thread* pop_cola_new() {
    pthread_mutex_lock(&MUTEX_LISTA_NEW);
    void* p = queue_pop(COLA_NEW);
    pthread_mutex_unlock(&MUTEX_LISTA_NEW);
    return (t_running_thread*) p;
}

void iterar_cola_new(void (*function)(void*)) {
    pthread_mutex_lock(&MUTEX_LISTA_NEW);
    list_iterate(COLA_NEW->elements, function);
    pthread_mutex_unlock(&MUTEX_LISTA_NEW);
}

void* remover_cola_new(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_LISTA_NEW);
    obj_tid = tid;
    void* p = list_remove_by_condition(COLA_NEW->elements, filter_t_running_thread_by_tid);
    pthread_mutex_unlock(&MUTEX_LISTA_NEW);
    return p;
}

uint16_t largo_cola_new() {
    pthread_mutex_lock(&MUTEX_LISTA_NEW);
    uint16_t ret = queue_size(COLA_NEW);
    pthread_mutex_unlock(&MUTEX_LISTA_NEW);
    return ret;
}

// Cola tripulantes

void push_cola_tripulante(t_running_thread* t) {
    pthread_mutex_lock(&MUTEX_COLA);
    queue_push(COLA_TRIPULANTES, (void*) t);
    sem_post(&TRIPULANTES_EN_COLA);
    pthread_mutex_unlock(&MUTEX_COLA);
    (t->t)->status = READY;
}

t_running_thread* pop_cola_tripulante() {
    pthread_mutex_lock(&MUTEX_COLA);
    void* t = queue_pop(COLA_TRIPULANTES);
    pthread_mutex_unlock(&MUTEX_COLA);
    return (t_running_thread*) t;
}

t_running_thread* buscar_cola_tripulante(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_COLA);
    obj_tid = tid;
    void* p = list_find(COLA_TRIPULANTES->elements, filter_t_running_thread_by_tid);
    pthread_mutex_unlock(&MUTEX_COLA);
    return p;
}

uint16_t largo_cola() {
    pthread_mutex_lock(&MUTEX_COLA);
    uint16_t ret = queue_size(COLA_TRIPULANTES);
    pthread_mutex_unlock(&MUTEX_COLA);
    return ret;
}

void* remover_cola_tripulante(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_COLA);
    obj_tid = tid;
    void* p = list_remove_by_condition(COLA_TRIPULANTES->elements, filter_t_running_thread_by_tid);
    pthread_mutex_unlock(&MUTEX_COLA);
    return p;
}

void iterar_cola_ready(void (*function)(void*)) {
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

void iterar_lista_hilos(void (*f)(void*)) {
    pthread_mutex_lock(&MUTEX_LISTA_HILOS);
    list_iterate(LISTA_HILOS, f);
    pthread_mutex_unlock(&MUTEX_LISTA_HILOS);
}

void* buscar_lista_hilos(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_LISTA_HILOS);
    obj_tid = tid;
    void* p = list_find(LISTA_HILOS, filter_t_running_thread_by_tid);
    pthread_mutex_unlock(&MUTEX_LISTA_HILOS);
    return p;
}

void* remover_lista_hilos(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_LISTA_HILOS);
    obj_tid = tid;
    void* p = list_remove_by_condition(LISTA_HILOS, filter_t_running_thread_by_tid);
    pthread_mutex_unlock(&MUTEX_LISTA_HILOS);
    return p;
}

uint16_t largo_lista_hilos() {
    pthread_mutex_lock(&MUTEX_LISTA_HILOS);
    uint16_t ret = list_size(LISTA_HILOS);
    pthread_mutex_unlock(&MUTEX_LISTA_HILOS);
    return ret;
}

// Cola exit

void agregar_lista_exit(void* p) {
    pthread_mutex_lock(&MUTEX_COLA_EXIT);
    list_add(COLA_EXIT, p);
    pthread_mutex_unlock(&MUTEX_COLA_EXIT);
}

void* remover_lista_exit(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_COLA_EXIT);
    obj_tid = tid;
    void* p = list_remove_by_condition(COLA_EXIT, filter_by_tid);
    pthread_mutex_unlock(&MUTEX_COLA_EXIT);
    return p;
}

void iterar_lista_exit(void (*f)(void*)) {
    pthread_mutex_lock(&MUTEX_COLA_EXIT);
    list_iterate(COLA_EXIT, f);
    pthread_mutex_unlock(&MUTEX_COLA_EXIT);
}