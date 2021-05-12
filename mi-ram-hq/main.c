#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../shared/include/utils.h"
#include "../shared/include/sockets.h"
#include "include/graphic.h"
#include "include/init_mrhq.h"
#include "../shared/include/protocolo.h"

#define MODULENAME "MRH"
#define SERVERNAME "MRH_SERVER"
#define NIVELNAME "AMONG-OS"

static t_config_mrhq* initialize_cfg() {
    t_config_mrhq* cfg = malloc(sizeof(t_config_mrhq));
    cfg->ALGORITMO_REEMPLAZO = NULL;
    cfg->ESQUEMA_MEMORIA = NULL;
    cfg->PATH_SWAP = NULL;
    return cfg;
}

int main() {
    t_config_mrhq* cfg = initialize_cfg();
    t_log* logger = log_create("mi-ram-hq.log", MODULENAME, true, LOG_LEVEL_INFO);

    if(!cargar_configuracion(cfg, logger)) {
        cerrar_programa(cfg, logger);
        return EXIT_FAILURE;
    }

    // ****** CREACION DEL SERVIDOR ******
    char* puerto = string_itoa(cfg->PUERTO);
    int mrh_server = iniciar_servidor(logger, SERVERNAME, "127.0.0.1", puerto);
    free(puerto);

    // ****** INICIALIZACION DE LA GUI ******
    NIVEL* among_nivel = iniciar_gui(logger, NIVELNAME);

    while(server_escuchar(logger, SERVERNAME, mrh_server, among_nivel));

    nivel_destruir(among_nivel);
    liberar_conexion(&mrh_server);
    cerrar_programa(cfg, logger);

	return EXIT_SUCCESS;
}
