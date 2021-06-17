#ifndef ESTRUCTURAS_MRH_H_
#define ESTRUCTURAS_MRH_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t pid;
    uint32_t dl_tareas;
} PCB_t; // 8 bytes

typedef struct {
    uint32_t tid;
    char estado;
    uint32_t pos_x;
    uint32_t pos_y;
    uint32_t id_sig_tarea;
    uint32_t dl_pcb;
} TCB_t; // 21 bytes

////// SEGMENTACION //////

// TS === TABLA DE SEGMENTOS
typedef struct {
    uint8_t nro_segmento;
    uint32_t inicio;
    uint32_t tamanio;
} segmento_t;

typedef struct {
    uint32_t pid;
    segmento_t* pcb;
    segmento_t* tareas;
} ts_patota_t;      // solo tiene (siempre) esas 2 entradas

typedef struct {
    segmento_t* tcb;
} ts_tripulante_t;  // confeccionado como tabla pero solo tiene 1 entrada

///////// FUNCIONES /////////

/// Frees
void free_ts_patota_t(void* x);
void free_ts_tripulante_t(void* x);

/// Stringify
char* stringify_segmento_t(segmento_t* segmento);
char* stringify_ts_patota_t(ts_patota_t* tabla, char* timestamp);
char* stringify_ts_tripulante_t(ts_tripulante_t* tabla, char* timestamp);

/// Serializacion! De nuevo
void* serializar_pcb(PCB_t* pcb);
void* serializar_tcb(TCB_t* tcb);
void* serializar_string_tareas(char* string);

#endif
