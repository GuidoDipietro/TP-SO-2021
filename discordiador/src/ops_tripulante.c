#include "../include/ops_tripulante.h"

bool PLANIFICACION_BLOQUEADA = false;

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

static void reanudar_hilo(void* r_t) {
    sem_post(&(((t_running_thread*) r_t)->sem_pause));
    sem_post(&BLOQUEAR_PLANIFICADOR);
}

void bloquear_planificacion() {
    PLANIFICACION_BLOQUEADA = true;
    log_info(main_log, "Planificacion bloqueada");
}

void reanudar_planificacion() {
    PLANIFICACION_BLOQUEADA = false;
    iterar_lista_hilos(reanudar_hilo);

    while(largo_cola_new()) { // Movemos todos los hilos de NEW a READY
        t_running_thread* r_t = pop_cola_new();
        (r_t->t)->status = READY;
        push_cola_tripulante(r_t);
    }
    log_info(main_log, "Planificacion desbloqueada");
}

static t_tripulante* init_tripulante(t_posicion* pos, uint16_t pid) {
    t_tripulante* t = malloc(sizeof(t_tripulante));
    t->pid = pid;
    t->tid = generar_tid();
    t->status = NEW;
    t->pos = pos;
    t->tarea = NULL;

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
        log_error(main_log, "Error fatal al crear el tripulante %d en la patota %d", t->tid, t->pid);
        cerrar_conexiones_tripulante(t);
        free(port_i_mongo_store);
        free(port_mi_ram_hq);
        free_t_tripulante(t);
        return NULL;
    }

    free(port_i_mongo_store);
    free(port_mi_ram_hq);
    return t;
}

//
//
//

void cerrar_conexiones_tripulante(t_tripulante* t) {
    if (t->fd_mi_ram_hq) close(t->fd_mi_ram_hq);
    if (t->fd_i_mongo_store) close (t->fd_i_mongo_store);
}

t_tripulante* iniciar_tripulante(t_posicion* pos, uint16_t pid) {
    t_tripulante* t = init_tripulante(pos, pid);

    if(t == NULL)
        return NULL;
        
    uint8_t err = solicitar_tarea(t);

    if(err) {
        log_error(main_log, "No se pudo solicitar la tarea al crear el tripulante %d en la patota %d", t->tid, t->pid);
        cerrar_conexiones_tripulante(t);
        free_t_tripulante(t);
        return NULL;
    }

    return t;
}

uint8_t solicitar_tarea(t_tripulante* t) {
    // TODO: Aca se le pide la tarea al mi-ram-hq
    if(0) {
        return 1;
    }

    static uint16_t tareas = 0;

    if(tareas >= 7) {
        t->tarea = NULL;
        return 1;
    }

    t_tarea* tarea = malloc(sizeof(t_tarea));
    tarea->nombre = string_duplicate("Cositas pronto");
    tarea->tipo = OTRO_T;
    t_posicion* pos = malloc(sizeof(t_tarea));
    pos->x = 5;
    pos->y = 3;
    tarea->pos = pos;
    tarea->param = 0;
    tarea->duracion = 6;
    t->tarea = tarea;
    tareas++;

    return 0;
}