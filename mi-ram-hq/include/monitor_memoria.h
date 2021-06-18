#ifndef MONITOR_MEMORIA_H_
#define MONITOR_MEMORIA_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include <commons/bitarray.h>
#include <commons/temporal.h>

#include "init_mrhq.h"
#include "estructuras.h"

// init
void iniciar_mutex();
void finalizar_mutex();

/// SEGMENTACION

// utils
void dump_mp();
void memcpy_segmento_en_mp(uint32_t inicio, void* data, uint32_t size);
void memcpy_pagina_en_frame_mp(uint32_t nro_frame, void* data);
void memset_0_segmento_en_mp(uint32_t inicio, uint32_t tamanio);
void realloc_segmento_en_mp(uint32_t inicio, uint32_t destino, uint32_t tamanio);

segmento_t* new_segmento(uint8_t n, uint32_t inicio, uint32_t taman);
segmento_t* segmento_t_duplicate(segmento_t*);

/// SEGLIB
uint32_t list_size_seglib();
void list_add_seglib(segmento_t* seg);
t_list* list_filter_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_inicio_seglib(uint32_t inicio);
segmento_t* list_add_all_holes_seglib();
segmento_t* list_get_seglib(uint32_t indice);
void list_clean_seglib();
void remove_zero_sized_gap_seglib();
void asesinar_seglib();
/// END SEGLIB

/// SEGUS
bool list_is_empty_segus();
void list_sort_segus();
void list_add_segus(segmento_t* seg);
void list_remove_by_inicio_segus(uint32_t inicio);
uint32_t list_size_segus();
segmento_t* list_find_by_inicio_segus(uint32_t inicio);
segmento_t* list_get_segus();
void asesinar_segus();
/// END SEGUS

/// FRAMBIT
int64_t primer_frame_libre_frambit();
void ocupar_frame_frambit(uint32_t index);
void liberar_frame_frambit(uint32_t index);
bool estado_frame_frambit(uint32_t index);
/// END FRAMBIT

// debug
void print_bitarray_frames(bool);
void print_segmento_t(void*);
void print_seglib(bool);
void print_segus(bool);

#endif
