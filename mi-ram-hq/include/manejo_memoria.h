#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include "monitor_memoria.h"
#include "estructuras.h"

extern t_config_mrhq* cfg;

// Ver tests para ejemplos :) :D

/// SEGMENTACION

bool entra_en_mp(uint32_t tamanio);
bool meter_segmento_en_mp(void* data, size_t size);
bool mover_segmento_en_mp(uint32_t inicio_origen, uint32_t inicio_destino);

bool meter_segmento_actualizar_hueco(segmento_t* hueco_target, uint32_t tamanio);
segmento_t* proximo_hueco_best_fit(uint32_t tamanio);
segmento_t* proximo_hueco_first_fit(uint32_t tamanio);
void compactar_segmentos_libres(void);

/// PAGINACION

#endif