#ifndef TP_2021_1C_UNDEFINED_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_TRIPULANTE_H

#include "../../shared/include/protocolo.h"
#include "logs.h"
#include "config.h"
#include "../include/config.h"
#include <stdlib.h>
#include <commons/collections/queue.h>
#include "../../shared/include/frees.h"
#include "../../shared/include/sockets.h"

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

void free_t_tripulante(void*);
uint8_t iniciar_tripulante(t_posicion*);

#endif
