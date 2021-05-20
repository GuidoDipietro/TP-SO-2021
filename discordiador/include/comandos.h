#ifndef COMMANDOS_H_
#define COMANDOS_H_

#include <stdint.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "../../shared/include/utils.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/frees.h"
#include "../include/logs.h"
#include "ops_tripulante.h"

void iniciar_patota(char*, int*, int*);
void listar_tripulantes(char*, int*, int*);
void expulsar_tripulante(char*, int*, int*);
void iniciar_planificacion(char*, int*, int*);
void pausar_planificacion(char*, int*, int*);
void obtener_bitacora(char*, int*, int*);

#endif