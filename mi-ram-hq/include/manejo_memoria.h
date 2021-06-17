#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include <inttypes.h>
#include "init_mrhq.h"
#include "monitor_memoria.h"
#include "estructuras.h"

// Ver tests para ejemplos :) :D

/// SEGMENTACION

bool entra_en_mp(uint32_t tamanio);
uint32_t meter_segmento_en_mp(void* data, size_t size);
bool eliminar_segmento_de_mp(uint32_t inicio);
bool compactar_mp();

bool meter_segmento_actualizar_hueco(segmento_t* hueco_target, uint32_t tamanio);
segmento_t* proximo_hueco_best_fit(uint32_t tamanio);
segmento_t* proximo_hueco_first_fit(uint32_t tamanio);
void compactar_segmentos_libres(void);

/// PAGINACION

#endif
