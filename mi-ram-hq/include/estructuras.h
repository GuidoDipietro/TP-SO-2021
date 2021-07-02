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
} TCB_t; // 21 bytes (sin padding)

////// SEGMENTACION //////

// TS === TABLA DE SEGMENTOS
// segmento_t se usa como entrada en lista de huecos, lista de segmentos usados, y para las tablas
typedef enum {
    TAREAS_SEG,
    PCB_SEG,
    TCB_SEG,
} tipo_segmento_t;

typedef struct {
    tipo_segmento_t tipo;
    uint32_t inicio;
    uint32_t tamanio;
} segmento_t;

typedef struct {
    uint32_t pid;
    uint32_t tripulantes_totales;
    uint32_t tripulantes_inicializados;
    t_list* posiciones;
    segmento_t* pcb;
    segmento_t* tareas;
} ts_patota_t;

typedef struct {
    uint32_t tid;
    segmento_t* tcb;
} ts_tripulante_t;

////// PAGINACION //////

// TP === TABLA DE PAGINAS
typedef struct {
    uint32_t nro_pagina;    // la pagina X
    uint32_t nro_frame;     // esta en el frame Y
    union {
        struct {
            uint16_t bit_U; // bit usado
            uint16_t bit_M; // bit modificado
        };
        uint32_t TUR;       // tiempo de ultima referencia
    };
} entrada_tp_t;

typedef struct {
    uint32_t pid;
    uint32_t tripulantes_totales;
    uint32_t tripulantes_inicializados;
    t_list* posiciones;
    t_list* tabla;          // tipo: <entrada_tp_t>
} tp_patota_t;

typedef union {
    uint32_t bytes;
    struct {
        unsigned lleno          :  1;
        unsigned amedias        :  1;
        unsigned pid_ocupador   : 30;
    };
} frame_t;

///////// FUNCIONES /////////

/// Creacion
tp_patota_t* tp_patota_t_create();

/// Frees
void free_ts_patota_t(void* x);
void free_ts_tripulante_t(void* x);
void free_tp_patota_t(void* x);

/// Stringify
char* stringify_segmento_t(segmento_t* segmento);
char* stringify_ts_patota_t(ts_patota_t* tabla, char* timestamp);
char* stringify_ts_tripulante_t(ts_tripulante_t* tabla, char* timestamp);

/// Serializacion! De nuevo
void* serializar_pcb(PCB_t* pcb);
PCB_t* deserializar_pcb(void* stream);

void* serializar_tcb(TCB_t* tcb);
TCB_t* deserializar_tcb(void* stream);

void* serializar_string_tareas(char* string);

#endif
