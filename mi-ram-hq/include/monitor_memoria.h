#ifndef MONITOR_MEMORIA_H_
#define MONITOR_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <commons/temporal.h>

#include "init_mrhq.h"
#include "estructuras.h"

// init
void iniciar_mutex();
void finalizar_mutex();

/// SEGMENTACION

// utils
void dump_mp(int);
void memcpy_segmento_en_mp(uint32_t inicio, void* data, size_t size);

segmento_t* new_segmento(uint8_t n, uint32_t inicio, uint32_t taman);
segmento_t* segmento_t_duplicate(segmento_t*);

/// SEGLIB
void list_add_seglib(segmento_t* seg);
t_list* list_filter_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_inicio_seglib(uint32_t inicio);
segmento_t* list_add_all_holes_seglib();
void list_clean_seglib();
void remove_zero_sized_gap_seglib();
void asesinar_seglib();
/// END SEGLIB

/// SEGUS
void list_add_segus(segmento_t* seg);
void asesinar_segus();
/// END SEGUS

// debug
void print_segmento_t(void*);
void print_seglib();
void print_segus();

#endif