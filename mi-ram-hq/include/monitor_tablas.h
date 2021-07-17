#ifndef MONITOR_TABLAS_H_
#define MONITOR_TABLAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <commons/memory.h>

#include "init_mrhq.h"
#include "estructuras.h"

/// TS PATOTAS
void list_add_tspatotas(ts_patota_t* elem);
void list_delete_by_pid_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_pid_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_pid_plus_plus_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_inicio_pcb_tspatotas(uint32_t inicio);
ts_patota_t* list_find_by_inicio_tareas_tspatotas(uint32_t inicio);
void asesinar_tspatotas();


/// TS TRIPULANTES
void list_update_nro_seg_tcb_by_pid_tstripulantes(uint32_t tid_eliminado, uint32_t pid);
ts_tripulante_t* list_find_by_tid_tstripulantes(uint32_t tid);
ts_tripulante_t* list_find_by_inicio_tcb_tstripulantes(uint32_t inicio);
void list_delete_by_tid_tstripulantes(uint32_t tid);
void list_add_tstripulantes(ts_tripulante_t* elem);
void asesinar_tstripulantes();

/// TP PATOTAS

void list_add_tppatotas(tp_patota_t* elem);
tp_patota_t* list_remove_by_pid_tppatotas(uint32_t pid);
tp_patota_t* list_find_by_pid_tppatotas(uint32_t pid);
tp_patota_t* list_find_by_pid_plus_plus_tppatotas(uint32_t pid);
void list_add_page_frame_tppatotas(uint32_t pid, uint32_t nro_frame, size_t size, bool presente);
void list_update_page_frame_tppatotas(uint32_t pid, uint32_t nro_pag, uint32_t nro_frame);
void list_update_n_of_pages_tppatotas(uint32_t pid, uint32_t n_pags);
void list_indicar_pagina_en_frame_tppatotas(uint32_t pid, uint32_t nro_pagina, uint32_t nuevo_frame);
uint32_t list_get_frame_of_page_tppatotas(uint32_t pid, uint32_t page);
uint32_t list_get_n_of_pages_tppatotas(uint32_t pid);
void asesinar_tppatotas();

/// TID_PID_LOOKUP
uint32_t pid_of_tid(uint32_t tid);
tid_pid_lookup_t* list_tid_pid_lookup_find_by_tid(uint32_t tid);
void list_add_tid_pid_lookup(tid_pid_lookup_t* elem);
void list_tid_pid_lookup_remove_by_tid(uint32_t tid);
void asesinar_tid_pid_lookup();

/// DEBUG

void print_tspatotas(bool log);
void print_tstripulantes(bool log);
void print_tppatotas(bool log);
void print_tid_pid_lookup(bool log);
void print_swap(bool log);

/// Prints (logs)
#define PRI_MP                 (uint16_t) 0b0000000000000001
#define PRI_SEGLIB             (uint16_t) 0b0000000000000010
#define PRI_SEGUS              (uint16_t) 0b0000000000000100
#define PRI_TSTRIPULANTES      (uint16_t) 0b0000000000001000
#define PRI_TSPATOTAS          (uint16_t) 0b0000000000010000
#define PRI_FRAMO              (uint16_t) 0b0000000000100000
#define PRI_TPPATOTAS          (uint16_t) 0b0000000001000000
#define PRI_TID_PID_LOOKUP     (uint16_t) 0b0000000010000000
#define PRI_SWAP               (uint16_t) 0b0000000100000000
#define PRI_ALL_SEGMENTACION   (uint16_t) 0b0000000000011111
#define PRI_NONE               (uint16_t) 0b0000000000000000
#define PRI_ALL_PAGINACION     (uint16_t) 0b0000000111100001
#define PRI_AUTO               (uint16_t) 0b1000000000000000
#define LOGPRINT(_)            print_ ## _ (true)
void log_structures(uint16_t options);

#endif
