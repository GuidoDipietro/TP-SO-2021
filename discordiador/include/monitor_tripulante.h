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
} t_tripulante;

// si se usa FIFO va a ser una t_queue*, si se usa RR va a ser una t_list*
extern t_queue* COLA_TRIPULANTES;

void push_cola_tripulante(t_tripulante*);

#endif
