#ifndef MONITOR_MEMORIA_H_
#define MONITOR_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "init_mrhq.h"
#include "estructuras.h"

// init
void iniciar_mutex();
void finalizar_mutex();

// utils
segmento_t* new_segmento(uint32_t inicio, uint32_t taman);
segmento_t* segmento_t_duplicate(segmento_t*);

void list_add_seglib(segmento_t* seg);
t_list* list_filter_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_inicio_seglib(uint32_t inicio);
segmento_t* list_add_all_holes_seglib();
void list_clean_seglib();
void remove_zero_sized_gap_seglib();

void asesinar_seglib();

// debug
void print_seglib();

#endif