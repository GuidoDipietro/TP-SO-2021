#ifndef MONITOR_TABLAS_H_
#define MONITOR_TABLAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "init_mrhq.h"
#include "estructuras.h"

/// TS PATOTAS
void list_add_tspatotas(ts_patota_t* elem);
void asesinar_tspatotas();


/// TS TRIPULANTES
void list_add_tstripulantes(ts_tripulante_t* elem);
void asesinar_tstripulantes();

#endif
