#include "include/main.h"

extern t_config_mrhq* cfg;
extern t_log* logger;
extern t_list* segmentos_libres;

extern void* memoria_principal;

extern sem_t SEM_COMPACTACION_DONE;
extern sem_t SEM_COMPACTACION_START;

void sighandler(int x) {
    switch (x) {
        case SIGUSR1:
        {
            // dump_mp();
            char* dumpcito = mem_hexstring(memoria_principal, 2048);
            log_info(logger, "%s", dumpcito);
            free(dumpcito);
            print_seglib(true);
            print_segus(true);
            print_tspatotas(true);
            print_tstripulantes(true);
            break;
        }
        case SIGUSR2:
            sem_post(&SEM_COMPACTACION_START);
            compactar_mp();
            sem_wait(&SEM_COMPACTACION_DONE);
            break;
    }
}

int main() {
    signal(SIGUSR1, sighandler);
    signal(SIGUSR2, sighandler);

    if(!init() || !cargar_configuracion("mi-ram-hq.config") || !cargar_memoria()) {
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
