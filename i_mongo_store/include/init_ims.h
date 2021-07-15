#ifndef TP_2021_1C_UNDEFINED_INIT_H
#define TP_2021_1C_UNDEFINED_INIT_H

#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>

#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"

#include "init_fs.h"
#include "blocks.h"
#include "monitor_bitarray.h"
#include "monitor_lista_archivos.h"
typedef struct {
    char* PUNTO_MONTAJE;
    uint16_t PUERTO;
    uint32_t TIEMPO_SINCRONIZACION;
    t_list* POSICIONES_SABOTAJE;
    uint32_t BLOCKS;
    uint32_t BLOCK_SIZE;
} t_config_ims;

extern t_config_ims* cfg;

uint8_t cargar_configuracion();
bool crear_servidor(int* fd, char* name);
void cerrar_programa();
void iniciar_sincronizador();

#endif
