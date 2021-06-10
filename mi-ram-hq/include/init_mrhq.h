#ifndef TP_2021_1C_UNDEFINED_INIT_H_IMS
#define TP_2021_1C_UNDEFINED_INIT_H_IMS

#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../include/graphic.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"
#include <string.h>

typedef struct {
    uint16_t TAMANIO_MEMORIA;
    char* ESQUEMA_MEMORIA;
    uint16_t TAMANIO_PAGINA;
    uint16_t TAMANIO_SWAP;
    char* PATH_SWAP;
    char* ALGORITMO_REEMPLAZO;
    char* CRITERIO_SELECCION;
    uint16_t PUERTO;
    uint16_t IP;
} t_config_mrhq;

extern t_log* logger;
extern t_config_mrhq* cfg;

uint8_t cargar_configuracion();
void cerrar_programa();
int server_escuchar(char*, int);

#endif
