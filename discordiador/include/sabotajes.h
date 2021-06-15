#ifndef TP_2021_1C_UNDEFINED_SABOTAJES_H
#define TP_2021_1C_UNDEFINED_SABOTAJES_H

#include <signal.h>
#include "monitor_tripulante.h"


#define SIG_SABOTAJE SIGUSR1
#define SIG_FIN_SABOTAJE SIGUSR2

void iniciar_sabotaje(uint16_t signum);
void finalizar_sabotaje(uint16_t signum);
void set_signal_handlers();

#endif
