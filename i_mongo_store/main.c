#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "include/init_ims.h"

#include "../shared/include/utils.h"
#include "../shared/include/sockets.h"
#include "../shared/include/protocolo.h"

#define MODULENAME "I-MONGO-STORE"
#define SERVERNAME "IMS_SERVER"

static t_config_ims* initialize_cfg() {
    t_config_ims* cfg = malloc(sizeof(t_config_ims));
    cfg->PUNTO_MONTAJE = NULL;
    cfg->POSICIONES_SABOTAJE = NULL;
    return cfg;
}

int main() {
    t_config_ims* cfg = initialize_cfg();

    t_log* logger = log_create("i_mongo_store.log", MODULENAME, true, LOG_LEVEL_INFO);

    int server_fd;

    if(!cargar_configuracion(cfg, logger) || !crear_servidor(&server_fd, SERVERNAME, cfg, logger)) {
        cerrar_programa(logger, cfg);
        return EXIT_FAILURE;
    }

    // Envio y recepcion de mensajes perenne
    while (server_escuchar(logger, SERVERNAME, server_fd));

    cerrar_programa(logger, cfg);

    return EXIT_SUCCESS;
}
