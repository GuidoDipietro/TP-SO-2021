#ifndef TEST_MEMORIA_H_
#define TEST_MEMORIA_H_

#include <CUnit/Basic.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/memory.h>

#include "../../mi-ram-hq/include/init_mrhq.h"
#include "../../mi-ram-hq/include/estructuras.h"
#include "../../mi-ram-hq/include/manejo_memoria.h"
#include "../../mi-ram-hq/include/monitor_memoria.h"

void iniciar();
void finalizar();

#endif