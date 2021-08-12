#include "include/main.h"

static t_config_ims* initialize_cfg() {
    t_config_ims* cfg = malloc(sizeof(t_config_ims));
    cfg->PUNTO_MONTAJE = NULL;
    cfg->POSICIONES_SABOTAJE = NULL;
    return cfg;
}

t_log* logger;
t_list* OPEN_FILES;
int server_fd;

int main() {
    cfg = initialize_cfg();
    OPEN_FILES = list_create();
    logger = log_create("i_mongo_store.log", MODULENAME, true, LOG_LEVEL_INFO);

    if(!cargar_configuracion() || !crear_servidor(&server_fd, SERVERNAME)) {
        liberar_conexion(&server_fd);
        return EXIT_FAILURE;
    }

    pthread_t HILO_CONTROLADOR;
    pthread_create(&HILO_CONTROLADOR, NULL, (void*) controlador_disco, NULL);
    pthread_detach(HILO_CONTROLADOR);

    cargar_superbloque();
    cargar_bloques();
    iniciar_sincronizador();
    set_signal_handlers();

    // Envio y recepcion de mensajes perenne
    while (server_escuchar(SERVERNAME, server_fd));

    return EXIT_SUCCESS;
}
