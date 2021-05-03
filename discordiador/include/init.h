#ifndef TP_2021_1C_UNDEFINED_INIT_H
#define TP_2021_1C_UNDEFINED_INIT_H

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../shared/include/sockets.h"
#include "../../shared/include/utils.h"

typedef struct {
    char* IP_MI_RAM_HQ;
    uint16_t PUERTO_MI_RAM_HQ;
    char* IP_I_MONGO_STORE;
    uint16_t PUERTO_I_MONGO_STORE;
    uint16_t GRADO_MULTITAREA;
    char* ALGORITMO;
    uint16_t QUANTUM;
    uint16_t DURACION_SABOTAJE;
    uint16_t RETARDO_CICLO_CPU;
} config;

uint8_t cargar_configuracion(config*);
uint8_t generar_conexiones(int*, int*, config*);
void cerrar_programa(t_log*, t_log*, config*);
#endif