#ifndef ESTRUCTURAS_MRH_H_
#define ESTRUCTURAS_MRH_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/list.h>

 // 8 bytes
typedef struct {
    uint32_t pid;
    uint32_t dl_tareas; // segmentacion = offset en void* | paginacion = 0
} PCB_t;

 // 21 bytes (sin padding)
typedef struct {
    uint32_t tid;
    char estado;
    uint32_t pos_x;
    uint32_t pos_y;
    uint32_t id_sig_tarea;
    uint32_t dl_pcb;    // segmentacion = offset en void* | paginacion = [N PAG | N OFFSET] 16|16
} TCB_t;

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
    uint32_t nro_segmento;
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
        uint32_t bit_U;     // bit de uso
        uint32_t TUR;       // tiempo de ultima referencia
    };
    bool bit_P;             // bit de presencia
} entrada_tp_t;

typedef struct {
    uint32_t pid;
    uint32_t tripulantes_totales;
    uint32_t tripulantes_inicializados;
    uint32_t tamanio_tareas;
    uint32_t pages;
    t_list* posiciones;
    t_list* paginas;          // tipo: <entrada_tp_t>
} tp_patota_t;

typedef struct {
    uint32_t tid;
    uint32_t pid;
    uint32_t nro_pagina;      // pagina del TCB
    uint32_t inicio;          // inicio del TCB en esa pagina (puede pasar a la proxima)
} tid_pid_lookup_t;

typedef struct {
    uint32_t pid;
    uint32_t nro_pagina;
    uint32_t inicio;
} frame_swap_t;
// con los 4 bytes extra de padding me hago alto guiso

typedef union {
    uint64_t bytes;
    struct {
        unsigned libre          :  1;
        unsigned amedias        :  1;
        unsigned inicio_hueco   : 30;
        unsigned pid_ocupador   : 32;
    };
} frame_t;

///////// FUNCIONES /////////

/// Frees
void free_ts_patota_t(void* x);
void free_ts_tripulante_t(void* x);
void free_tp_patota_t(void* x);

/// Serializacion! De nuevo
void* serializar_pcb(PCB_t* pcb);
PCB_t* deserializar_pcb(void* stream);

void* serializar_tcb(TCB_t* tcb);
TCB_t* deserializar_tcb(void* stream);

void* serializar_string_tareas(char* string);

#endif
