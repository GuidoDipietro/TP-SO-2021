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

uint8_t cargar_configuracion(t_config_ims*, t_log*);
bool crear_servidor(int* fd, char* name, t_config_ims* cfg, t_log* logger);
void server_escuchar(t_log* logger, char* server_name, int server_fd);
void cerrar_programa(t_log*, t_config_ims*);

#endif
