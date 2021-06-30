#ifndef MONITOR_TABLAS_H_
#define MONITOR_TABLAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "init_mrhq.h"
#include "estructuras.h"

/// TS PATOTAS
void list_add_tspatotas(ts_patota_t* elem);
void list_delete_by_pid_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_pid_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_pid_plus_plus_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_inicio_pcb_tspatotas(uint32_t inicio);
void asesinar_tspatotas();


/// TS TRIPULANTES
ts_tripulante_t* list_find_by_tid_tstripulantes(uint32_t tid);
ts_tripulante_t* list_find_by_inicio_tcb_tstripulantes(uint32_t inicio);
void list_delete_by_tid_tstripulantes(uint32_t tid);
void list_add_tstripulantes(ts_tripulante_t* elem);
void asesinar_tstripulantes();

/// TP PATOTAS

void asesinar_tppatotas();

/// DEBUG

void print_tspatotas(bool log);
void print_tstripulantes(bool log);

#endif
