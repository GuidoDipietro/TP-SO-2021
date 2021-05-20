#include "./include/console.h"
#include "./include/init_disc.h"
#include "./include/logs.h"
#include "../shared/include/protocolo.h"
#include "include/monitor_tripulante.h"
#include <commons/collections/queue.h>
#include "include/config.h"

#define MODULENAME "DIS"

t_log* main_log;
t_log* main_log_inv;
t_queue* COLA_TRIPULANTES;
t_config_disc* DISCORDIADOR_CFG;

static t_config_disc* initialize_cfg() {
    t_config_disc* cfg = malloc(sizeof(t_config_disc));
    cfg->ALGORITMO = NULL;
    cfg->IP_I_MONGO_STORE = NULL;
    cfg->IP_MI_RAM_HQ = NULL;
    return cfg;
}

static void print_t_tripulante(void* t_p) {
    t_tripulante* t = (void*) t_p;
    printf("\nPID: %d", t->pid);
    printf("\nTID: %d", t->tid);
    printf("\nSTATUS: %d\n", t->status);
    print_t_posicion(t->pos);
    print_t_tarea(t->tarea);
    printf("\n");
}

int main() {
    DISCORDIADOR_CFG = initialize_cfg();
    COLA_TRIPULANTES = queue_create();

    main_log = log_create("discordiador.log", "DISCORDIADOR", true, LOG_LEVEL_INFO);
    main_log_inv = log_create("discordiador.log", "DISCORDIADOR", false, LOG_LEVEL_TRACE);

    // Mirar el return code de cargar_configuracion
    int i_mongo_store_fd, mi_ram_hq_fd;

    if(!cargar_configuracion(DISCORDIADOR_CFG) || !generar_conexiones(&i_mongo_store_fd, &mi_ram_hq_fd, DISCORDIADOR_CFG)) {
        cerrar_programa(main_log, main_log_inv, DISCORDIADOR_CFG);
        return EXIT_FAILURE;
    }

    menu_start(&i_mongo_store_fd, &mi_ram_hq_fd);
    
    list_iterate(COLA_TRIPULANTES->elements, print_t_tripulante);

    cerrar_programa(main_log, main_log_inv, DISCORDIADOR_CFG);

	return EXIT_SUCCESS;
}
