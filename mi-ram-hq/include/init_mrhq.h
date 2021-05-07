#ifndef TP_2021_1C_UNDEFINED_INIT_H_IMS
#define TP_2021_1C_UNDEFINED_INIT_H_IMS

#include <stdint.h>
#include <commons/log.h>
#include "../../shared/include/utils.h"
#include <string.h>

typedef struct {
    uint16_t TAMANIO_MEMORIA;
    char* ESQUEMA_MEMORIA;
    uint16_t TAMANIO_PAGINA;
    uint16_t TAMANIO_SWAP;
    char* PATH_SWAP;
    char* ALGORITMO_REEMPLAZO;
    uint16_t PUERTO;
} t_config_mrhq;

uint8_t cargar_configuracion(t_config_mrhq*, t_log*);
void cerrar_programa(t_config_mrhq*, t_log*);

#endif