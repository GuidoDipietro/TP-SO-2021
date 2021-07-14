#include "include/main.h"

static t_config_ims* initialize_cfg() {
    t_config_ims* cfg = malloc(sizeof(t_config_ims));
    cfg->PUNTO_MONTAJE = NULL;
    cfg->POSICIONES_SABOTAJE = NULL;
    return cfg;
}

t_log* logger;

int main() {
    cfg = initialize_cfg();
    logger = log_create("i_mongo_store.log", MODULENAME, true, LOG_LEVEL_INFO);

    int server_fd;

    if(!cargar_configuracion() || !crear_servidor(&server_fd, SERVERNAME)) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

<<<<<<< HEAD
    //cargar_superbloque();
    //cargar_bloques();
=======
    cargar_superbloque();
    cargar_bloques();
    iniciar_sincronizador();
>>>>>>> 1baebe2cbd9177067afe172998a7332143d91f44

    // Envio y recepcion de mensajes perenne
    while (server_escuchar(SERVERNAME, server_fd));

    cerrar_programa();

    return EXIT_SUCCESS;
}
