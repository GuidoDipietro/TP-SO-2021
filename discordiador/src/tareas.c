#include "../include/tareas.h"

static void imprimir_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;

    // 8 espacios
    printf(
        "\nTripulante: %3d        Patota: %3d",
        t->tid, t->pid
    );
}

// Para imprimir los tripulantes que se estan ejecutando
static void imprimir_tripulante_exec(void* t_p) {
    imprimir_tripulante(((t_running_thread*) t_p)->t);
}

static bool posiciones_iguales(t_posicion* p1, t_posicion* p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

static int16_t signo(int16_t val) {
    return (0 < val) - (val < 0);
}

bool PLANIFICADOR_ALIVE = false;

void planificador() {
    PLANIFICADOR_ALIVE = true;
    sem_init(&ACTIVE_THREADS, 0, DISCORDIADOR_CFG->GRADO_MULTITAREA);
    sem_init(&BLOQUEAR_PLANIFICADOR, 0, 0);
    while (1) {
        if (PLANIFICACION_BLOQUEADA)
            sem_wait(&BLOQUEAR_PLANIFICADOR);

        t_running_thread* new = pop_cola_tripulante();
        // Preparamos el hilo para correr
        (new->t)->status = EXEC;
        new->blocked = false;
        monitor_add_lista_hilos((void*) new);
        sem_post(&(new->sem_pause)); // Para arrancar el loop
    }
    PLANIFICADOR_ALIVE = false;
    printf("\nFin planificacion\n");
}

/*
    correr_tripulante_FIFO y correr_tripulante_RR son la misma funcion, nada mas que
    correr_tripulante_RR agrega la logica del algoritmo RR

*/

void correr_tripulante_FIFO(t_running_thread* thread_data) {
    t_tripulante* t = thread_data->t;

    sem_wait(&(thread_data->sem_pause));
    // Este semaforo es para que empiece a correr

    while (1) {
        if(PLANIFICACION_BLOQUEADA)
            sem_wait(&(thread_data->sem_pause));

        if(thread_data->blocked)
            sem_wait(&(thread_data->sem_pause));

        //ciclo();

        if((t->tarea)->duracion)
            correr_tarea(thread_data);
        else {
            if(replanificar_tripulante(thread_data, t)) {
                log_info(main_log, "El tripulante %d no tiene mas tareas pendientes.", t->tid);
                goto final;
            } else
                log_info(main_log, "El tripulante %d fue replanificado", t->tid);
        }
    }

    final:
        cerrar_conexiones_tripulante(t);
        agregar_lista_exit(t);
        sem_destroy(&(thread_data->sem_pause));
        free(thread_data);
}   

void correr_tripulante_RR(t_running_thread* thread_data) {
    t_tripulante* t = thread_data->t;

    sem_wait(&(thread_data->sem_pause));
    // Este semaforo es para que empiece a correr

    while (1) {
        if(PLANIFICACION_BLOQUEADA)
            sem_wait(&(thread_data->sem_pause));

        if(thread_data->blocked)
            sem_wait(&(thread_data->sem_pause));

        //ciclo();

        if ((t->tarea)->duracion) {
            if(thread_data->quantum == DISCORDIADOR_CFG->QUANTUM)
                desalojar_tripulante(thread_data);
            else {
                correr_tarea(thread_data);
                (thread_data->quantum)++;
            }
        }
        else {
            if(replanificar_tripulante(thread_data, t)) {
                log_info(main_log, "El tripulante %d no tiene mas tareas pendientes.", t->tid);
                goto final;
            } else
                log_info(main_log, "El tripulante %d fue replanificado", t->tid);
        }
    }

    final:
        cerrar_conexiones_tripulante(t);
        agregar_lista_exit(t);
        sem_destroy(&(thread_data->sem_pause));
        free(thread_data);
}   

void desalojar_tripulante(t_running_thread* thread_data) {
    log_info(main_log, "El tripulante %d fue desalojado por fin de quantum", (thread_data->t)->tid);
    remover_lista_hilos((thread_data->t)->tid);
    thread_data->blocked = true;
    thread_data->quantum = 0;
    sem_post(&ACTIVE_THREADS);
    (thread_data->t)->status = READY;
    push_cola_tripulante(thread_data);
}

uint8_t replanificar_tripulante(t_running_thread* thread_data, t_tripulante* t) {
    thread_data->blocked = true;
    remover_lista_hilos(t->tid);
    sem_post(&ACTIVE_THREADS);
    free_t_tarea(t->tarea); // Limpiamos la tarea vieja

    if(solicitar_tarea(t))
        return 1;

    thread_data->quantum = 0;
    push_cola_tripulante(thread_data);
    t->status = READY;
    return 0;
}

void correr_tarea(t_running_thread* r_t) {
    t_tripulante* t = r_t->t;

    if(!posiciones_iguales(t->pos, (t->tarea)->pos)) {
        t_posicion* origen = malloc(sizeof(t_posicion));
        origen->x = t->pos->x;
        origen->y = t->pos->y;

        int16_t dif = (t->pos)->x - ((t->tarea)->pos)->x;

        if(dif != 0)
            (t->pos)->x -= signo(dif);
        else {
            dif = (t->pos)->y - ((t->tarea)->pos)->y;
            (t->pos)->y -= signo(dif);
        }

        // Le avisamos al MRH que actualice la GUI
        int ret_code = send_movimiento(
            t->fd_mi_ram_hq,
            t->tid,
            origen,
            t->pos
        );
        if (ret_code == -1) {
            log_error(main_log, "Error enviando movimiento del tripulante %d", t->tid);
        }

        log_info(main_log, "MOVE tid#%d %d|%d => %d|%d",
            t->tid, origen->x, origen->y, t->pos->x, t->pos->y
        );

        free(origen);
    } else {
        ((t->tarea)->duracion)--; // Decrementamos hasta que no tenga mas duracion
        log_info(main_log, "WAIT tid#%d @ %d|%d (dur: %d)",
            t->tid, t->pos->x, t->pos->y, t->tarea->duracion
        );
    }
}