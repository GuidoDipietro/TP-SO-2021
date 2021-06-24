#ifndef MONITOR_TABLAS_H_
#define MONITOR_TABLAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "init_mrhq.h"
#include "estructuras.h"

/// TS PATOTAS
void list_add_tspatotas(ts_patota_t* elem);
ts_patota_t* list_find_by_pid_tspatotas(uint32_t pid);
ts_patota_t* list_find_by_pid_plus_plus_tspatotas(uint32_t pid);
void asesinar_tspatotas();


/// TS TRIPULANTES
void list_add_tstripulantes(ts_tripulante_t* elem);
void asesinar_tstripulantes();

/// TP PATOTAS

void asesinar_tppatotas();

/// DEBUG

void print_tspatotas(bool log);
void print_tstripulantes(bool log);

#endif
