#ifndef TP_2021_1C_UNDEFINED_INIT_H
#define TP_2021_1C_UNDEFINED_INIT_H

#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"

typedef struct {
    char* PUNTO_MONTAJE;
    uint16_t PUERTO;
    uint32_t TIEMPO_SINCRONIZACION;
    t_list* POSICIONES_SABOTAJE;
} t_config_ims;

uint8_t cargar_configuracion();
bool crear_servidor(int* fd, char* name);
void cerrar_programa();

#endif
