#ifndef TEST_PROTOCOLO_H_
#define TEST_PROTOCOLO_H_

#include <CUnit/Basic.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <stdlib.h>
#include "../../shared/include/protocolo.h"
#include "../../shared/include/sockets.h"

void crear_conexiones();
void cerrar_conexiones();

#endif