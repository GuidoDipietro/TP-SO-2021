#include "./include/console.h"
#include "./include/init_disc.h"
#include "./include/logs.h"
#include "../shared/include/protocolo.h"

#define MODULENAME "DIS"

t_log* main_log;
t_log* main_log_inv;

static t_config_disc* initialize_cfg() {
    t_config_disc* cfg = malloc(sizeof(t_config_disc));
    cfg->ALGORITMO = NULL;
    cfg->IP_I_MONGO_STORE = NULL;
    cfg->IP_MI_RAM_HQ = NULL;
    return cfg;
}

int main() {
    t_config_disc* cfg = initialize_cfg();

    main_log = log_create("discordiador.log", "DISCORDIADOR", true, LOG_LEVEL_INFO);
    main_log_inv = log_create("discordiador.log", "DISCORDIADOR", false, LOG_LEVEL_TRACE);

    // Mirar el return code de cargar_configuracion
    int i_mongo_store_fd, mi_ram_hq_fd;

    if(!cargar_configuracion(cfg) || !generar_conexiones(&i_mongo_store_fd, &mi_ram_hq_fd, cfg)) {
        cerrar_programa(main_log, main_log_inv, cfg);
        return EXIT_FAILURE;
    }

    menu_start(&i_mongo_store_fd, &mi_ram_hq_fd);

    cerrar_programa(main_log, main_log_inv, cfg);

	return EXIT_SUCCESS;
}
