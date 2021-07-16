#ifndef TP_2021_1C_UNDEFINED_MONITOR_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_MONITOR_TRIPULANTE_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "../../shared/include/structures.h"
#include "../../shared/include/protocolo.h"
#include "logs.h"
//#include "ops_tripulante.h"

// Semaforo del planificador

sem_t ACTIVE_THREADS;
sem_t TRIPULANTES_EN_COLA;
sem_t TRIPULANTE_LISTA_HILOS_PAUSADO;
sem_t TRIPULANTE_EN_BLOQUEADOS;

//

typedef struct {
    uint32_t tid;
    uint32_t pid;
    t_status status;
    t_tarea* tarea;
    t_posicion* pos;
    int fd_i_mongo_store;
    int fd_mi_ram_hq;
} t_tripulante;

typedef struct {
    t_tripulante* t;
    pthread_t thread;
    uint16_t quantum;
    sem_t sem_pause;
    bool blocked;
} t_running_thread;

// Alguna util

void cambiar_estado(t_tripulante* p, t_status nuevo);

// Cola NEW

extern t_queue* COLA_NEW;

void push_cola_new(t_running_thread*);
t_running_thread* pop_cola_new();
void iterar_cola_new(void (*f)(void*));
uint16_t largo_cola_new();
t_running_thread* buscar_cola_new(uint32_t tid);
void* remover_cola_new(uint32_t tid);

// Bloqueados E/S

extern t_queue* COLA_BLOQUEADOS;

void push_cola_bloqueados(t_running_thread* thread);
t_running_thread* pop_cola_bloqueados();
void iterar_cola_bloqueados(void (*f)(void*));
t_running_thread* buscar_cola_bloqueados(uint32_t tid);

// Cola tripulantes

extern t_queue* COLA_TRIPULANTES;

void push_cola_tripulante(t_running_thread*);
t_running_thread* pop_cola_tripulante();
t_running_thread* buscar_cola_tripulante(uint32_t);
void* remover_cola_tripulante(uint32_t); 
void iterar_cola_ready(void (*f)(void*));
uint16_t largo_cola();

// Lista hilos

extern t_list* LISTA_HILOS;

void iniciar_mutex();

void monitor_add_lista_hilos(void*); 
void* monitor_remove_by_condition_lista_hilos(bool (*f)(void*));
uint16_t largo_lista_hilos();
t_running_thread* buscar_lista_hilos(uint32_t tid);
void* remover_lista_hilos(uint32_t);
void iterar_lista_hilos(void (*f)(void*));

// Hilos tripulantes

void bloquear_tripulantes();
void desbloquear_tripulantes();

// Cola exit

extern t_list* COLA_EXIT;

void agregar_lista_exit(void*);
void iterar_lista_exit(void (*f)(void*));
void* remover_lista_exit(uint32_t tid);
t_tripulante* obtener_lista_exit(uint32_t tid);

// Cola BLOCKED SABOTAJE

extern t_list* LISTA_SABOTAJE;
pthread_mutex_t MUTEX_LISTA_SABOTAJE;

#endif