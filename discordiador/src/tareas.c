#include "../include/tareas.h"

static bool posiciones_iguales(t_posicion* p1, t_posicion* p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

static int16_t signo(int16_t val) {
    return (0 < val) - (val < 0);
}

static uint16_t tid_cmp = 0;
static bool filter_by_tid(void* t_p) {
    t_running_thread* t = (t_running_thread*) t_p;
    return (t->t)->tid == tid_cmp;
}

// TODO: Monitor de la LISTA_HILOS

//
// Public functions
//

// Este es el loop principal del planificador
void planificador() {
    sem_init(&active_threads, 0, DISCORDIADOR_CFG->GRADO_MULTITAREA);
    log_info(main_log, "Planificacion iniciada");
    while(largo_cola() != 0) {
        sem_wait(&active_threads);
        t_running_thread* thread = malloc(sizeof(t_running_thread));
        thread->t = pop_cola_tripulante();
        pthread_create(
            &(thread->thread),
            NULL,
            (void*) correr_tarea_FIFO,
            thread
        );
        pthread_detach(thread->thread);
        monitor_add_lista_hilos((void*) thread);
    }
    //list_destroy(LISTA_HILOS); // Los elementos de la lista los elimina el hilo mismo
    sem_destroy(&active_threads);
}

// Esta funcion es donde se corre la tarea de cada tripulante
bool BLOCKED_THREADS = false;

void correr_tarea_FIFO(t_running_thread* r_t) {
    t_tripulante* t = r_t->t;
    t->status = EXEC;
    // Primero nos movemos a la posicion correcta
    while(1) {
        sleep(2);

        if(BLOCKED_THREADS)
            sem_wait(&SEM_BLOCKED_THREADS);

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

            log_info(main_log, "Mover tid#%d %d|%d => %d|%d",
                t->tid, origen->x, origen->y, t->pos->x, t->pos->y
            );

            free(origen);
        } else {
            if((t->tarea)->duracion > 0) {
                ((t->tarea)->duracion)--; // Decrementamos hasta que no tenga mas duracion
                log_info(main_log, "tid#%d @ %d|%d (dur: %d)",
                    t->tid, t->pos->x, t->pos->y, t->tarea->duracion
                );
            }
            else { // Termino la tarea
                // Lo sacamos de la lista de hilos activos
                log_info(main_log, "tid#%d DISMISSED", t->tid);
                t->status = EXIT;
                free_t_tarea(t->tarea);

                // Removemos de la lista de hilos y limpiamos el nodo
                tid_cmp = t->tid;
                monitor_remove_by_condition_lista_hilos(filter_by_tid);
                free((t_running_thread*) r_t); // Limpiamos el nodo de la lista de hilos

                reasignar_tripulante(t);
                sem_post(&active_threads); // Actualizamos el semaforo, marcando que hay un nuevo hilo disponible
                return;
            }
        }
    }
}

void reasignar_tripulante(t_tripulante* t) {
    uint8_t ret = solicitar_tarea(t);

    if (ret) { // Borrar el tripulante, no hay mas tareas que hacer
        cerrar_conexiones_tripulante(t);
        free_t_tripulante(t);
        // TODO: avisar al MRH que se murio el tripulante
        return;
    }

    push_cola_tripulante(t); // Lo volvemos a agregar a la cola
}