#include "include/main.h"

int main() {
    cfg = initialize_cfg();
    logger = log_create("mi-ram-hq.log", MODULENAME, false, LOG_LEVEL_INFO);

    if(!cargar_configuracion() || !cargar_memoria()) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    // ****** CREACION DEL SERVIDOR ******
    char* puerto = string_itoa(cfg->PUERTO);
    int mrh_server = iniciar_servidor(logger, SERVERNAME, "127.0.0.1", puerto);
    free(puerto);

    // ****** INICIALIZACION DE LA GUI ******
    iniciar_gui(NIVELNAME);

    while(server_escuchar(SERVERNAME, mrh_server));

    nivel_destruir(among_nivel);
    liberar_conexion(&mrh_server);
    cerrar_programa();

    return EXIT_SUCCESS;
}
