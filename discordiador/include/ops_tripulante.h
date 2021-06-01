#ifndef TP_2021_1C_UNDEFINED_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_TRIPULANTE_H

#include "../../shared/include/protocolo.h"
#include "logs.h"
#include "config.h"
#include "../include/config.h"
#include <stdlib.h>
#include <pthread.h>
#include "../../shared/include/frees.h"
#include "../../shared/include/sockets.h"
#include "monitor_tripulante.h"

bool PLANIFICACION_BLOQUEADA;

void cerrar_conexiones_tripulante(t_tripulante*);
void free_t_tripulante(void*);
t_tripulante* iniciar_tripulante(t_posicion*, uint16_t);
uint8_t solicitar_tarea(t_tripulante*);
//uint8_t op_expulsar_tripulante(uint16_t);
void correr_tripulante(t_running_thread*);

void bloquear_planificacion();
void reanudar_planificacion();

uint16_t generar_pid();
uint16_t generar_tid();

#endif
