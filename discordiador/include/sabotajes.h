#ifndef TP_2021_1C_UNDEFINED_SABOTAJES_H
#define TP_2021_1C_UNDEFINED_SABOTAJES_H

#include <signal.h>
#include <stdbool.h>
#include <math.h>
#include "monitor_tripulante.h"
#include "logs.h"

#define SIG_SABOTAJE SIGUSR1
#define SIG_FIN_SABOTAJE SIGUSR2

extern bool SABOTAJE_ACTIVO;

void iniciar_sabotaje(int signum);
void finalizar_sabotaje(int signum);
void set_signal_handlers();

#endif
