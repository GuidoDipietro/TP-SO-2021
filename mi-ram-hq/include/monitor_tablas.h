#ifndef MONITOR_TABLAS_H_
#define MONITOR_TABLAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <commons/memory.h>

#include "init_mrhq.h"
#include "estructuras.h"

extern void* memoria_principal; // solo por un print

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
void list_add_page_frame_tppatotas(uint32_t pid, uint32_t nro_frame);
void list_update_page_frame_tppatotas(uint32_t pid, uint32_t nro_pag, uint32_t nro_frame);
void list_update_n_of_pages_tppatotas(uint32_t pid, uint32_t n_pags);
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
void print_tid_pid_lookup(bool log);

/// Prints (logs)
#define PRI_MP                 (uint8_t) 0b00000001
#define PRI_SEGLIB             (uint8_t) 0b00000010
#define PRI_SEGUS              (uint8_t) 0b00000100
#define PRI_TSTRIPULANTES      (uint8_t) 0b00001000
#define PRI_TSPATOTAS          (uint8_t) 0b00010000
#define PRI_FRAMO              (uint8_t) 0b00100000
#define PRI_TPPATOTAS          (uint8_t) 0b01000000
#define PRI_TID_PID_LOOKUP     (uint8_t) 0b10000000
#define PRI_ALL_SEGMENTACION   (uint8_t) 0b00011111
#define PRI_NONE               (uint8_t) 0b00000000
#define PRI_ALL_PAGINACION     (uint8_t) 0b11100001
#define LOGPRINT(_)            print_ ## _ (true)
void log_structures(uint8_t options);

#endif
