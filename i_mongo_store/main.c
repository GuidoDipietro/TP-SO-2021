#include "include/main.h"

static t_config_ims* initialize_cfg() {
    t_config_ims* cfg = malloc(sizeof(t_config_ims));
    cfg->PUNTO_MONTAJE = NULL;
    cfg->POSICIONES_SABOTAJE = NULL;
    return cfg;
}

t_log* logger;
t_list* OPEN_FILES;

int main() {
    cfg = initialize_cfg();
    OPEN_FILES = list_create();
    logger = log_create("i_mongo_store.log", MODULENAME, true, LOG_LEVEL_INFO);

    int server_fd;

    if(!cargar_configuracion() || !crear_servidor(&server_fd, SERVERNAME)) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    cargar_superbloque();
    cargar_bloques();
    iniciar_sincronizador();

    //crear_archivo("Oxigeno.ims", 'O');
    //cargar_archivo("Oxigeno.ims");

    // Envio y recepcion de mensajes perenne
    while (server_escuchar(SERVERNAME, server_fd));

    cerrar_programa();

    return EXIT_SUCCESS;
}
