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
char* stringify_tspatotas();
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

/// Prints (logs)
#define PRI_MP              0x00000001
#define PRI_SEGLIB          0x00000010
#define PRI_SEGUS           0x00000100
#define PRI_TSTRIPULANTES   0x00001000
#define PRI_TSPATOTAS       0x00010000
#define PRI_ALL             0x11111111
#define PRI_ALL_NO_MP       0x11111110
#define PRI_NONE            0x00000000
void log_structures(uint8_t options);

#endif
