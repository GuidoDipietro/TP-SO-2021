#ifndef MONITOR_MEMORIA_H_
#define MONITOR_MEMORIA_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/temporal.h>

#include "init_mrhq.h"
#include "estructuras.h"

// init
void iniciar_mutex();
void finalizar_mutex();

/// SEGMENTACION

// utils
void memcpy_segmento_en_mp(uint32_t inicio, void* data, uint32_t size);
void* get_segmento_data(uint32_t inicio, uint32_t size);
void memset_0_segmento_en_mp(uint32_t inicio, uint32_t tamanio);
void realloc_segmento_en_mp(uint32_t inicio, uint32_t destino, uint32_t tamanio);
segmento_t* new_segmento(tipo_segmento_t tipo, uint32_t nro_segmento, uint32_t inicio, uint32_t taman);
segmento_t* segmento_t_duplicate(segmento_t*);

	// NO SE CONTEMPLO NADA DE SWAP TODAVIA
void* get_pagina_data(uint32_t nro_frame);
void memcpy_pagina_en_frame_mp(uint32_t nro_frame, uint32_t inicio, void* data, size_t size);
void clear_frame_en_mp(uint32_t nro_frame);

		         /* ------------------------------------------------------------ */

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
void unificar_huecos_seglib();
void asesinar_seglib();
/// END SEGLIB

/// SEGUS
bool list_is_empty_segus();
void list_sort_segus();
void list_add_segus(segmento_t* seg);
void list_remove_by_inicio_segus(uint32_t inicio);
uint32_t list_size_segus();
segmento_t* list_find_by_inicio_segus(uint32_t inicio);
segmento_t* list_get_segus(uint32_t indice);
t_list* list_get_tcb_segments_segus();
t_list* list_get_pcb_segments_segus();
char* stringify_segus();
void asesinar_segus();
/// END SEGUS

/// FRAMO
uint32_t primer_frame_libre_framo(uint32_t pid, uint32_t* inicio);
uint32_t cant_frames_libres();
void ocupar_frame_framo(uint32_t index, size_t size, uint32_t pid);
void liberar_frame_framo(uint32_t nro_frame);
char* stringify_tabla_frames();
/// END FRAMO

// debug
void print_framo(bool);
void print_segmento_t(void*);
void print_seglib(bool);
void print_segus(bool);

#endif
