#ifndef TP_2021_1C_UNDEFINED_INIT_H_IMS
#define TP_2021_1C_UNDEFINED_INIT_H_IMS

#include <stdint.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <signal.h>

typedef struct {
    char* ESQUEMA_MEMORIA;
    char* PATH_SWAP;
    char* ALGORITMO_REEMPLAZO;
    char* CRITERIO_SELECCION;
    uint16_t TAMANIO_MEMORIA;
    uint16_t TAMANIO_PAGINA;
    uint16_t TAMANIO_SWAP;
    uint16_t PUERTO;
    uint16_t IP;
    // agregadas:
    uint16_t CANT_PAGINAS;
    bool SEG;
} t_config_mrhq;

static t_config_mrhq* initialize_cfg() {
    t_config_mrhq* cfg = malloc(sizeof(t_config_mrhq));
    cfg->ALGORITMO_REEMPLAZO = NULL;
    cfg->ESQUEMA_MEMORIA = NULL;
    cfg->PATH_SWAP = NULL;
    return cfg;
}

#include "monitor_memoria.h"
#include "monitor_tablas.h"
#include "manejo_memoria.h"
#include "graphic.h"
#include "estructuras.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"

#define MODULENAME "MRH"

uint8_t init();                 // inicializa loger, cfg, y semaforos
uint8_t cargar_configuracion(); // carga cfg en strut cfg
uint8_t cargar_memoria();       // Init de segmentos_libres

void cerrar_programa();
int server_escuchar(char*, int);

#endif
