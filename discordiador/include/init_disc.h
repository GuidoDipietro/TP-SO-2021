#ifndef TP_2021_1C_UNDEFINED_INIT_H
#define TP_2021_1C_UNDEFINED_INIT_H

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../shared/include/sockets.h"
#include "../../shared/include/utils.h"
#include "config.h"
#include "tripulante.h"

uint8_t cargar_configuracion(t_config_disc*);
uint8_t generar_conexiones(int*, int*, t_config_disc*);
void cerrar_programa(t_log*, t_log*, t_config_disc*);
#endif