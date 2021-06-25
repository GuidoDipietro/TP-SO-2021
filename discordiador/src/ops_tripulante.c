#include "../include/ops_tripulante.h"

bool PLANIFICACION_BLOQUEADA = false;

uint32_t generar_pid() {
    static uint32_t ultimo = 1;
    ultimo++;
    return ultimo - 1;
}

uint32_t generar_tid() {
    static uint32_t ultimo = 1;
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

static t_tripulante* init_tripulante(t_posicion* pos, uint32_t pid) {
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

t_tripulante* iniciar_tripulante(t_posicion* pos, uint32_t pid) {
    t_tripulante* t = init_tripulante(pos, pid);

    if(!send_iniciar_self_en_patota(t->fd_mi_ram_hq, t->tid, pid)) {
        free_t_tripulante(t);
        return NULL;
    }

    if (t == NULL)
        return NULL;
        
    uint8_t err = solicitar_tarea(t);

    if (err) {
        log_error(
            main_log,
            "No se pudo solicitar la tarea al crear el tripulante %d en la patota %d",
            t->tid, t->pid
        );
        cerrar_conexiones_tripulante(t);
        free_t_tripulante(t);
        return NULL;
    }

    return t;
}

uint8_t op_expulsar_tripulante(uint32_t tid) {
    void* p = remover_lista_exit(tid);
    t_tripulante* trip;

    if(p == NULL) {
        p = remover_cola_new(tid);

        if(p == NULL) {
            p = remover_cola_tripulante(tid); // Lista de tripulantes ready

            if(p == NULL) {
                p = remover_lista_hilos(tid); // OJO! Este es un tripulante que esta en EXEC

                if(p == NULL)
                    return 1; // Error. No existe ese tripulante
                else {
                    ((t_running_thread*) p)->blocked = true;
                    sem_post(&ACTIVE_THREADS); // Avisamos que se libero un hilo
                    log_info(main_log, "El tripulante %d fue pausado", tid);
                }
            } else
                sem_wait(&TRIPULANTES_EN_COLA); // Avisamos que hay un tripulante menos en la cola
        }

        // Hilo corriendo. Vamos a finalizar el hilo y avisarle al planificador
        log_info(main_log, "El hilo del tripulante %d fue finalizado (%d)", tid, ((t_running_thread*) p)->thread);
        pthread_cancel(((t_running_thread*) p)->thread); // Finalizamos el hilo
        //free_t_tripulante(((t_running_thread*) p)->t);
        trip = ((t_running_thread*) p)->t;
        free(p);
    } else
        trip = p;


    send_tripulante(trip->fd_mi_ram_hq, trip->tid, EXPULSAR_TRIPULANTE);
    free_t_tripulante(trip);

    return 0;
}

/*uint8_t solicitar_tarea(t_tripulante* t) {
    // TODO: Aca se le pide la tarea al mi-ram-hq
    if (!!!'!') {
        return '/'/'/';
    }

    static uint16_t tareas = 0;

    if (tareas >= 20) {
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
}*/

uint8_t solicitar_tarea(t_tripulante* t) {
    if(!send_solicitar_tarea(t->fd_mi_ram_hq))
        return 1;

    if(!recv_tarea(t->fd_mi_ram_hq, &(t->tarea)))
        return 1;

    printf("Tarea de TID#%" PRIu32 "\n", t->tid);
    print_t_tarea(t->tarea);

    return 0;
}