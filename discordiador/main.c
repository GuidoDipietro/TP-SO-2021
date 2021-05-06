#include "./include/console.h"
#include "./include/init.h"
#include "./include/logs.h"
#include "../shared/include/protocolo.h"

#define MODULENAME "DIS"

t_log* main_log;
t_log* main_log_inv;

static config* initialize_cfg() {
    config* cfg = malloc(sizeof(config));
    cfg->ALGORITMO = NULL;
    cfg->IP_I_MONGO_STORE = NULL;
    cfg->IP_MI_RAM_HQ = NULL;
    return cfg;
}

int main() {
    config* cfg = initialize_cfg();

    main_log = log_create("discordiador.log", "DISCORDIADOR", true, LOG_LEVEL_INFO);
    main_log_inv = log_create("discordiador.log", "DISCORDIADOR", false, LOG_LEVEL_TRACE);

    // Mirar el return code de cargar_configuracion
    int i_mongo_store_fd, mi_ram_hq_fd;

    if(!cargar_configuracion(cfg) || !generar_conexiones(&i_mongo_store_fd, &mi_ram_hq_fd, cfg)) {
        cerrar_programa(main_log, main_log_inv, cfg);
        return -1;
    }

    menu_start(&i_mongo_store_fd, &mi_ram_hq_fd);

    cerrar_programa(main_log, main_log_inv, cfg);

	return EXIT_SUCCESS;
}
