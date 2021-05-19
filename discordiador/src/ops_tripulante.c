#include "../include/ops_tripulante.h"

void free_t_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;
    if(t->pos != NULL)
        free_t_posicion(t->pos);

    free(t);
}

uint8_t iniciar_tripulante(t_posicion* pos) {
    t_tripulante* t = malloc(sizeof(t_tripulante));
    t->status = NEW;
    t->pos = malloc(sizeof(t_posicion));
    t->tarea = NULL;
    memcpy(t->pos, pos, sizeof(t_posicion));

    char* port_i_mongo_store = string_itoa(DISCORDIADOR_CFG->PUERTO_I_MONGO_STORE);
    char* port_mi_ram_hq = string_itoa(DISCORDIADOR_CFG->PUERTO_MI_RAM_HQ);

    t->fd_i_mongo_store = crear_conexion(
            main_log,
            "I_MONGO_STORE",
            DISCORDIADOR_CFG->IP_I_MONGO_STORE,
            port_i_mongo_store
    );

    t->fd_mi_ram_hq = crear_conexion(
            main_log,
            "MI_RAM_HQ",
            DISCORDIADOR_CFG->IP_MI_RAM_HQ,
            port_mi_ram_hq
    );

    if(t->fd_mi_ram_hq == 0 || t->fd_i_mongo_store == 0) {
        free(port_i_mongo_store);
        free(port_mi_ram_hq);
        free_t_tripulante(t);
        return 1;
    }

    free(port_i_mongo_store);
    free(port_mi_ram_hq);

    log_info(main_log, "Tripulante creado");
    push_cola_tripulante(t);
    return 0;
}