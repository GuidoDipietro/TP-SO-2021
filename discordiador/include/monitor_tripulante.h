#ifndef TP_2021_1C_UNDEFINED_MONITOR_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_MONITOR_TRIPULANTE_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include "../../shared/include/protocolo.h"

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

#endif
