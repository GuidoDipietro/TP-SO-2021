#include "./include/console.h"
#include "./include/init.h"
#include "./include/logs.h"

#define MODULENAME "DIS"

t_log* main_log;
t_log* main_log_inv;

int main() {
    config* cfg = malloc(sizeof(config));
    main_log = log_create("discordiador.log", "DISCORDIADOR", true, LOG_LEVEL_INFO);
    main_log_inv = log_create("discordiador.log", "DISCORDIADOR", false, LOG_LEVEL_TRACE);

    // Mirar el return code de cargar_configuracion
    int i_mongo_store_fd, mi_ram_hq_fd;

    if(!cargar_configuracion(cfg) || !generar_conexiones(&i_mongo_store_fd, &mi_ram_hq_fd, cfg)) {
        cerrar_programa(main_log, main_log_inv, cfg);
        return -1;
    }

    menu_start();

    cerrar_programa(main_log, main_log_inv, cfg);

	return EXIT_SUCCESS;
}
