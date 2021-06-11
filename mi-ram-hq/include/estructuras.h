#ifndef ESTRUCTURAS_MRH_H_
#define ESTRUCTURAS_MRH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t pid;
    uint32_t dl_tareas;
} PCB_t;

typedef struct {
    uint32_t tid;
    char estado;
    uint32_t pos_x;
    uint32_t pos_y;
    uint32_t id_sig_tarea;
    uint32_t dl_pcb;
} TCB_t;

////// SEGMENTACION //////

// TS SIGNIFICA TABLA DE SEGMENTOS
typedef struct {
    uint32_t inicio;
    uint32_t tamanio;
} segmento_t;

typedef struct {
    segmento_t* tabla[2];
} ts_patota_t; // solo tiene (siempre) 2 entradas

typedef struct {
    segmento_t* tabla;
} ts_tripulante_t; // confeccionado como tabla pero solo tiene 1 entrada

///////// FUNCIONES /////////

extern t_list* segmentos_libres;

segmento_t* proximo_hueco_best_fit(uint32_t tamanio);
segmento_t* proximo_hueco_first_fit(uint32_t tamanio);

#endif
