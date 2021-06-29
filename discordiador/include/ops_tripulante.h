#ifndef TP_2021_1C_UNDEFINED_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_TRIPULANTE_H

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>

#include "logs.h"
#include "config.h"
#include "monitor_tripulante.h"

#include "../../shared/include/protocolo.h"
#include "../../shared/include/frees.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/structures.h"


bool PLANIFICACION_BLOQUEADA;
sem_t BLOQUEAR_PLANIFICADOR;

void cerrar_conexiones_tripulante(t_tripulante*);
void free_t_tripulante(void*);
t_tripulante* iniciar_tripulante(t_posicion*, uint32_t);
uint8_t solicitar_tarea(t_tripulante*);
uint8_t op_expulsar_tripulante(uint32_t);

void bloquear_planificacion();
void reanudar_planificacion();

uint32_t generar_pid();
uint32_t generar_tid();

#endif
