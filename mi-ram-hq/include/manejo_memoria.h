#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include "monitor_memoria.h"
#include "estructuras.h"

// Ver tests para ejemplos :) :D

bool meter_segmento_actualizar_hueco(segmento_t* hueco_target, uint32_t tamanio);
segmento_t* proximo_hueco_best_fit(uint32_t tamanio);
segmento_t* proximo_hueco_first_fit(uint32_t tamanio);
void compactar_segmentos_libres(void);

#endif