#ifndef TP_2021_1C_UNDEFINED_MONITOR_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_MONITOR_TRIPULANTE_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "../../shared/include/protocolo.h"
#include "logs.h"

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT
} t_status;

typedef struct {
    uint16_t tid;
    uint16_t pid;
    t_status status;
    t_tarea* tarea;
    t_posicion* pos;
    int fd_i_mongo_store;
    int fd_mi_ram_hq;
    uint16_t quantum; // Se va a usar solo si es RR
} t_tripulante;

extern t_queue* COLA_TRIPULANTES;

void push_cola_tripulante(t_tripulante*);
t_tripulante* pop_cola_tripulante();
t_tripulante* buscar_cola_tripulante(uint16_t);
void remover_cola_tripulante(uint16_t); 
void iterar_cola(void (*f)(void*));
uint16_t largo_cola();

// Lista hilos

typedef struct {
    t_tripulante* t;
    pthread_t thread;
} t_running_thread;

extern t_list* LISTA_HILOS;
sem_t SEM_BLOCKED_THREADS;
extern bool BLOCKED_THREADS;

void monitor_add_lista_hilos(void*); 
void* monitor_remove_by_condition_lista_hilos(bool (*f)(void*));
uint16_t largo_lista_hilos();
void* buscar_lista_hilos(uint16_t);
void remover_lista_hilos();
void iterar_lista_hilos(void (*f)(void*));

// Hilos tripulantes

void bloquear_tripulantes();
void desbloquear_tripulantes();

#endif
