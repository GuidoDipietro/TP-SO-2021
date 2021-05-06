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

    // prueba comunicacion

    // EXPULSAR TRIPULANTE EJEMPLO
    uint8_t tripulante_a_expulsar = 69;
    if (!send_tripulante(i_mongo_store_fd, 69, EXPULSAR_TRIPULANTE))
        log_error(main_log, "No pude enviar expulsar tripulante");

    // PATOTA DE EJEMPLO //
    uint8_t tripulantes = 6;
    char* filepath = "/home/etc/wow/such/a/long/path/name/holy/shites/ben/baron.txt";

    t_list* posiciones = list_create();
    t_posicion* p1 = malloc(sizeof(t_posicion));
    t_posicion* p2 = malloc(sizeof(t_posicion));
    t_posicion* p3 = malloc(sizeof(t_posicion));
    t_posicion* p4 = malloc(sizeof(t_posicion));
    t_posicion* p5 = malloc(sizeof(t_posicion));
    t_posicion* p6 = malloc(sizeof(t_posicion));
    p1->x=14; p2->x=27; p3->x=49; p4->x=73; p5->x=31; p6->x=99;
    p1->y= 6; p2->y= 9; p3->y= 7; p4->y= 1; p5->y= 2; p6->y= 0;
    list_add(posiciones, p1);
    list_add(posiciones, p2);
    list_add(posiciones, p3);
    list_add(posiciones, p4);
    list_add(posiciones, p5);
    list_add(posiciones, p6);

    // FIN PATOTA DE EJEMPLO //

    if (!send_patota(i_mongo_store_fd, tripulantes, filepath, posiciones))
        log_error(main_log, "No pude enviarlo correctamente.");
    //
    list_destroy_and_destroy_elements(posiciones, free_t_posicion);

    menu_start();

    cerrar_programa(main_log, main_log_inv, cfg);

	return EXIT_SUCCESS;
}
