#include "include/main.h"

extern t_config_mrhq* cfg;
extern t_log* logger;
extern t_list* segmentos_libres;

extern void* memoria_principal;

extern sem_t SEM_COMPACTACION_DONE;
extern sem_t SEM_COMPACTACION_START;

int mrh_server;

////// SIGNAL HANDLING //////
void sighandler(int x) {
    switch (x) {
        case SIGUSR1:
            dump_mp();
            log_structures(PRI_MP | PRI_FRAMO);
            break;
        case SIGUSR2:
            sem_post(&SEM_COMPACTACION_START);
            compactar_mp();
            sem_wait(&SEM_COMPACTACION_DONE);
            break;
        case SIGINT:
            liberar_conexion(&mrh_server);
            finalizar_gui();
            cerrar_programa();
            exit(EXIT_SUCCESS);
    }
}

////// MAIN //////

int main() {
    signal(SIGUSR1, sighandler);
    signal(SIGUSR2, sighandler);
    signal(SIGINT , sighandler);

    if(!init() || !cargar_configuracion("mi-ram-hq.config") || !cargar_memoria()) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    // ****** CREACION DEL SERVIDOR ******
    char* puerto = string_itoa(cfg->PUERTO);
    mrh_server = iniciar_servidor(logger, SERVERNAME, "127.0.0.1", puerto);
    free(puerto);

    // ****** INICIALIZACION DE LA GUI ******
    iniciar_gui(NIVELNAME);

    while (server_escuchar(SERVERNAME, mrh_server));

    liberar_conexion(&mrh_server);
    finalizar_gui();
    cerrar_programa();

    return EXIT_SUCCESS;
}