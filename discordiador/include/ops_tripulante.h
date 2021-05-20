#ifndef TP_2021_1C_UNDEFINED_TRIPULANTE_H
#define TP_2021_1C_UNDEFINED_TRIPULANTE_H

#include "../../shared/include/protocolo.h"
#include "logs.h"
#include "config.h"
#include "../include/config.h"
#include <stdlib.h>
#include "../../shared/include/frees.h"
#include "../../shared/include/sockets.h"
#include "monitor_tripulante.h"

void free_t_tripulante(void*);
uint8_t iniciar_tripulante(t_posicion*, uint16_t);

uint16_t generar_pid();
uint16_t generar_pid();

#endif
