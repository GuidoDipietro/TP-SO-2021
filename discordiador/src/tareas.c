#include "../include/tareas.h"

static bool PAUSED = false;
t_list* LISTA_HILOS;

static bool posiciones_iguales(t_posicion* p1, t_posicion* p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

static int16_t signo(uint16_t val) {
    return (0 < val) - (val < 0);
}

// Iterador para remover de una lista

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
    LISTA_HILOS = list_create();
    while(largo_cola() != 0) {
        sem_wait(&active_threads);
        t_running_thread* thread = malloc(sizeof(t_running_thread));
        thread->t = pop_cola_tripulante();
        pthread_create(
            &(thread->thread),
            NULL,
            (void*) correr_tarea_FIFO,
            thread->t
        );
        pthread_detach(thread->thread);
        list_add(LISTA_HILOS, (void*) thread);
    }
}

// Esta funcion es donde se corre la tarea de cada tripulante
void correr_tarea_FIFO(t_tripulante* t) {
    // Primero nos movemos a la posicion correcta
    while(1) {
        if(!posiciones_iguales(t->pos, (t->tarea)->pos)) {
            int16_t dif = (t->pos)->x - ((t->tarea)->pos)->x;

            if(dif != 0)
                (t->pos)->x += signo(dif);
            else {
                dif = (t->pos)->y - ((t->tarea)->pos)->y;
                (t->pos)->y += signo(dif);
            }
        } else {
            if((t->tarea)->duracion)
                ((t->tarea)->duracion)--; // Decrementamos hasta que no tenga mas duracion
            else { // Termino la tarea
                // Lo sacamos de la lista de hilos activos
                t->status = EXIT;
                tid_cmp = t->tid;
                void* p = list_remove_by_condition(LISTA_HILOS, filter_by_tid);
                free((t_running_thread*) p); // Limpiamos el nodo de la lista de hilos
                reasignar_tripulante(t);
                sem_post(&active_threads); // Actualizamos el semaforo, marcando que hay un nuevo hilo disponible
                return;
            }
        }
    }
}

void reasignar_tripulante(t_tripulante* t) {
    uint8_t ret = solicitar_tarea(t);

    if(ret) { // Borrar el tripulante, no hay mas tareas que hacer
        free_t_tripulante(t);
        return;
    }

    push_cola_tripulante(t); // Lo volvemos a agregar a la cola
}