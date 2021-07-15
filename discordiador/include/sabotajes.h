#ifndef TP_2021_1C_UNDEFINED_SABOTAJES_H
#define TP_2021_1C_UNDEFINED_SABOTAJES_H

#include <signal.h>
#include <stdbool.h>
#include <math.h>

#include "ciclo.h"
#include "monitor_tripulante.h"
#include "ops_tripulante.h"
#include "logs.h"
#include "tareas.h"
#include "../../shared/include/structures.h"
#include "config.h"
#include "listar_tripulantes.h"

#define SIG_SABOTAJE SIGUSR1

extern bool SABOTAJE_ACTIVO;

void iniciar_sabotaje(int signum);
void finalizar_sabotaje();
void set_signal_handlers();

#endif
