#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include <inttypes.h>
#include <semaphore.h>

#include "init_mrhq.h"
#include "monitor_memoria.h"
#include "estructuras.h"

// Ver tests para ejemplos :) :D

bool entra_en_mp(uint32_t tamanio);
bool entra_en_swap(uint32_t tamanio);
uint32_t paginas_que_entran_en_swap();
void dump_mp();

/// SEGMENTACION

bool get_structures_from_tabla_tripulante(ts_tripulante_t* tabla, TCB_t** p_tcb, PCB_t** p_pcb);
bool get_structures_from_tid_segmentacion
(uint32_t tid, ts_tripulante_t** p_tabla_tripulante, TCB_t** p_tcb, PCB_t** p_pcb);
uint32_t meter_segmento_en_mp(void* data, uint32_t size, tipo_segmento_t tipo);
bool eliminar_segmento_de_mp(uint32_t inicio);
bool compactar_mp();

bool meter_segmento_actualizar_hueco(segmento_t* hueco_target, uint32_t tamanio);
segmento_t* proximo_hueco_best_fit(uint32_t tamanio);
segmento_t* proximo_hueco_first_fit(uint32_t tamanio);
void compactar_segmentos_libres(void);

/// PAGINACION

void* RACE_read_from_mp_pid_pagina_offset_tamanio
(uint32_t pid, uint32_t pagina, uint32_t offset, uint32_t tamanio);		         // POSIBLE RC
bool RACE_get_structures_from_tid_paginacion
(uint32_t tid, tid_pid_lookup_t** p_tabla, TCB_t** p_tcb, PCB_t** p_pcb);        // POSIBLE RC
uint32_t append_data_to_patota_en_mp(void* data, size_t size, uint32_t pid, bool* nuevapag);
bool RACE_actualizar_tcb_en_mp(uint32_t pid, TCB_t* tcb);					     // POSIBLE RC
bool delete_patota_en_mp(uint32_t pid);

uint32_t pagina_a_reemplazar();
bool traer_pagina_de_swap(uint32_t pid, uint32_t nro_pagina);

#endif
