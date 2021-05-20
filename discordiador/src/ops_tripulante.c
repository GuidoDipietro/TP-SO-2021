#include "../include/ops_tripulante.h"

uint16_t generar_pid() {
    static uint16_t ultimo = 1;
    ultimo++;
    return ultimo - 1;
}

uint16_t generar_tid() {
    static uint16_t ultimo = 1;
    ultimo++;
    return ultimo - 1;
}

//
// Static functions
//

static t_tripulante* init_tripulante(t_posicion* pos, uint16_t pid) {
    t_tripulante* t = malloc(sizeof(t_tripulante));
    t->pid = pid;
    t->tid = generar_tid();
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
        log_error(main_log, "Error fatal al crear el tripulante %d en la patota %d", t->tid, t->pid);
        return NULL;
    }

    free(port_i_mongo_store);
    free(port_mi_ram_hq);
    return t;
}

//
// Public functions
//

void free_t_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;

    if(t->pos != NULL)
        free_t_posicion(t->pos);

    if(t->tarea != NULL)
        free_t_tarea(t->tarea);

    free(t);
}

uint8_t iniciar_tripulante(t_posicion* pos, uint16_t pid) {
    t_tripulante* t = init_tripulante(pos, pid);

    if(t == NULL) // Si t es NULL, error fatal en la creacion del tripulante
        return 1;

    // TODO: enviar bien el TID y PID al MI-RAM-HQ

    log_info(main_log, "Tripulante %d creado en la patota %d", t->pid, t->pid);
    uint8_t err = solicitar_tarea(t);

    if(err) {
        log_error(main_log, "No se pudo solicitar la tarea al crear el tripulante %d en la patota %d", t->tid, t->pid);
        return 1;
    }

    push_cola_tripulante(t);
    return 0;
}

uint8_t solicitar_tarea(t_tripulante* t) {
    // TODO: Aca se le pide la tarea al mi-ram-hq
    if(0) {
        return 1;
    }

    t_tarea* tarea = malloc(sizeof(t_tarea));
    tarea->nombre = "Cositas pronto";
    tarea->tipo = OTRO_T;
    t_posicion* pos = malloc(sizeof(t_tarea));
    pos->x = 5;
    pos->y = 3;
    tarea->pos = pos;
    tarea->param = 0;
    tarea->duracion = 6;
    t->tarea = tarea;
    t->status = READY;

    return 0;
}