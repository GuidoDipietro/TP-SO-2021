#ifndef TEST_PROTOCOLO_H_
#define TEST_PROTOCOLO_H_

#include <CUnit/Basic.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sysexits.h>
#include "../../shared/include/protocolo.h"
#include "../../shared/include/sockets.h"

int crear_conexiones();
int limpiar_conexiones();

#endif