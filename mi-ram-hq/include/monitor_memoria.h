#ifndef MONITOR_MEMORIA_H_
#define MONITOR_MEMORIA_H_

#include <stdlib.h>
#include <pthread.h>

#include "estructuras.h"

void iniciar_mutex();
void finalizar_mutex();

void list_add_seglib(segmento_t* seg);
t_list* list_filter_by_min_size_seglib(uint32_t min_size);
segmento_t* list_find_first_by_min_size_seglib(uint32_t min_size);
segmento_t* list_add_all_holes_seglib();
void list_clean_seglib();
void asesinar_seglib();

#endif