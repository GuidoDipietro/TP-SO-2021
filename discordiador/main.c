#include "./include/console.h"
#include "./include/init_disc.h"
#include "./include/logs.h"
#include "../shared/include/protocolo.h"
#include "include/monitor_tripulante.h"
#include <commons/collections/queue.h>
#include "include/config.h"
#include "include/sabotajes.h"
#include "include/entrada_salida.h"

#define MODULENAME "DIS"

t_log* main_log;
t_log* main_log_inv;
t_queue* COLA_TRIPULANTES;
t_list* LISTA_HILOS;
t_queue* COLA_NEW;
t_config_disc* DISCORDIADOR_CFG;
t_list* COLA_EXIT;
t_list* LISTA_SABOTAJE;
t_queue* COLA_BLOQUEADOS;

static t_config_disc* initialize_cfg() {
    t_config_disc* cfg = malloc(sizeof(t_config_disc));
    cfg->ALGORITMO = NULL;
    cfg->IP_I_MONGO_STORE = NULL;
    cfg->IP_MI_RAM_HQ = NULL;
    return cfg;
}

int main() {
    DISCORDIADOR_CFG = initialize_cfg();
    COLA_TRIPULANTES = queue_create();
    COLA_NEW = queue_create();
    LISTA_HILOS = list_create();
    COLA_EXIT = list_create();
    LISTA_SABOTAJE = list_create();
    COLA_BLOQUEADOS = queue_create();
    iniciar_mutex();

    main_log = log_create("discordiador.log", "DISCORDIADOR", true, LOG_LEVEL_INFO);
    main_log_inv = log_create("discordiador.log", "DISCORDIADOR", false, LOG_LEVEL_TRACE);

    // Mirar el return code de cargar_configuracion
    int i_mongo_store_fd, mi_ram_hq_fd;

    if(!cargar_configuracion(DISCORDIADOR_CFG) || !generar_conexiones(&i_mongo_store_fd, &mi_ram_hq_fd, DISCORDIADOR_CFG)) {
        cerrar_programa(main_log, main_log_inv, DISCORDIADOR_CFG);
        return EXIT_FAILURE;
    }

    pthread_t THREAD_IO;
    if(!pthread_create(&THREAD_IO, NULL, (void*) controlador_es, NULL))
        pthread_detach(THREAD_IO);
    else {
        log_error(main_log, "ERROR CRITICO INICIANDO EL DISCORDIADOR. NO SE PUDO CREAR EL HILO DEL CONTROLADOR DE E/S. ABORTANDO.");
        return EXIT_FAILURE;
    }

    pthread_t LISTENER_SABOTAJE;
     if(!pthread_create(&LISTENER_SABOTAJE, NULL, (void*) listener_sabotaje, NULL))
        pthread_detach(LISTENER_SABOTAJE);
    else {
        log_error(main_log, "ERROR CRITICO INICIANDO EL DISCORDIADOR. NO SE PUDO CREAR EL HILO DEL LISTENER DE SABOTAJES. ABORTANDO.");
        return EXIT_FAILURE;
    }

    menu_start(&i_mongo_store_fd, &mi_ram_hq_fd);
    
    cerrar_programa(main_log, main_log_inv, DISCORDIADOR_CFG);

	return EXIT_SUCCESS;
}
