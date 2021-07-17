#include "../include/sabotajes.h"

bool SABOTAJE_ACTIVO = false;

t_running_thread* encargado;
pthread_t HILO_PLANIFICADOR;

static void bloquear_hilo(void* p) {
    t_running_thread* t = (t_running_thread*) p;
    t->blocked = true;
    t->quantum = 0;
    sem_post(&ACTIVE_THREADS); // Liberamos cada tripulante corriendo
}

static bool sort_by_tid(void* p1, void* p2) {
    // Perdon, de verdad
    return (((t_running_thread*) p1)->t)->tid < (((t_running_thread*) p2)->t)->tid;
}

static void cambiar_status(void* p) {
    //(((t_running_thread*) p)->t)->status = BLOCKEDSAB;
    cambiar_estado(((t_running_thread*) p)->t, BLOCKEDSAB);
}

static void cambiar_status_ready(void* p) {
    //(((t_running_thread*) p)->t)->status = READY;
    cambiar_estado(((t_running_thread*) p)->t, READY);
    //((t_running_thread*) p)->blocked = false;
}

static bool comparator_remover_encargado(void* p) {
    return p == encargado;
}

// Esto es Haskell o C? Nadie lo sabe
void iniciar_sabotaje(t_tarea* tarea_sabotaje, int fd_sabotajes) {
    // Al estar todo pausado no deberiamos de preocuparnos por condiciones de carrera
    log_info(main_log, "Sabotaje detectado. Comenzando bloqueo por sabotaje.");
    SABOTAJE_ACTIVO = true; // Iniciamos todas las rutinas de sabotaje
    sem_post(&ACTIVE_THREADS);
    sem_post(&TRIPULANTES_EN_COLA);

    iterar_lista_hilos(bloquear_hilo); // Pausamos todos los hilos que esten corriendo

    for(uint8_t i = 0; i < largo_lista_hilos(); i++)
        sem_wait(&TRIPULANTE_LISTA_HILOS_PAUSADO); // Esperamos a que todos los hilos avisen que ya frenaron
    //int a;
    //sem_getvalue(&TRIPULANTES_EN_COLA, &a);

    list_sort(LISTA_HILOS, sort_by_tid); // Ordenamos la lista de hilos
    list_sort(COLA_TRIPULANTES->elements, sort_by_tid); // Ordenamos la cola de tripulantes

    //for(uint16_t i = 0; i < largo_cola(); i++) // Marcamos que no hay tripulantes en cola
    //    sem_wait(&TRIPULANTES_EN_COLA);

    // Bloqueando la LISTA_SABOTAJE nos aseguramos que los tripulantes que vuelven de IO entren ultimos
    pthread_mutex_lock(&MUTEX_LISTA_SABOTAJE);
    list_add_all(LISTA_SABOTAJE, LISTA_HILOS);
    list_clean(LISTA_HILOS);

    list_add_all(LISTA_SABOTAJE, COLA_TRIPULANTES->elements);
    queue_clean(COLA_TRIPULANTES);
    list_iterate(LISTA_SABOTAJE, cambiar_status);
    //op_listar_tripulantes();

    log_info(main_log, "Bloqueo por sabotaje finalizado.");

    // Ahora pasamos a resolver el sabotaje

    float calcular_distancia_a_sabotaje(void* p) {
        t_posicion* pos = (((t_running_thread*) p)->t)->pos;
        uint16_t difx = abs(tarea_sabotaje->pos->x - pos->x);
        uint16_t dify = abs(tarea_sabotaje->pos->y - pos->y);
        return sqrt(difx*difx+dify*dify);
    }

    void* comparar_cercania_a_sabotaje(void* p1, void* p2) {
        float d1 = calcular_distancia_a_sabotaje(p1);
        float d2 = calcular_distancia_a_sabotaje(p2);
        return d1 < d2 ? p1 : p2;
    }

    encargado = list_get_minimum(LISTA_SABOTAJE, comparar_cercania_a_sabotaje);
    send_tripulante((encargado->t)->fd_i_mongo_store, (encargado->t)->tid, ATENCION_SABOTAJE);
    if(!list_is_empty(LISTA_SABOTAJE)) {
        list_remove_by_condition(LISTA_SABOTAJE, comparator_remover_encargado); // Sacamos al encargado de esta lista

        log_info(main_log, "El tripulante %d es el encargado de resolver el sabotaje", (encargado->t)->tid);

        t_tarea* anterior = (encargado->t)->tarea;
        (encargado->t)->tarea = tarea_sabotaje;
        //(encargado->t)->status = EXEC;
        cambiar_estado(encargado->t, EXEC);
        monitor_add_lista_hilos(encargado);

        t_tripulante* t = encargado->t;

        while(((encargado->t)->tarea)->duracion) {
            ciclo_dis();
            if(!posiciones_iguales(t->pos, (t->tarea)->pos))
                mover_tripulante(encargado);
            else {
                correr_tarea_generica(encargado);
            }
        }
        send_iniciar_fsck(fd_sabotajes);

        log_info(main_log, "Sabotaje resuelto");
        (encargado->t)->tarea = anterior;
    } else {
        send_iniciar_fsck(fd_sabotajes);
        log_info(main_log, "No hay tripulantes disponibles para resolver los sabotajes. Salteando sabotaje");
    }
    
    pthread_mutex_unlock(&MUTEX_LISTA_SABOTAJE); // Aca ya podemos liberar la LISTA_SABOTAJE

    //free_t_tarea((encargado->t)->tarea);
    // Volvemos a meter al encargado en la cola de bloqueados
    remover_lista_hilos(((encargado)->t)->tid);
    //(encargado->t)->status = BLOCKEDSAB;
    cambiar_estado(encargado->t, BLOCKEDSAB);
    list_add(LISTA_SABOTAJE, encargado);
    send_tripulante((encargado->t)->fd_i_mongo_store, (encargado->t)->tid, RESOLUCION_SABOTAJE);

    finalizar_sabotaje();
}

void finalizar_sabotaje() {
    pthread_mutex_lock(&MUTEX_LISTA_SABOTAJE);
    list_destroy(COLA_TRIPULANTES->elements);
    COLA_TRIPULANTES->elements = LISTA_SABOTAJE;
    LISTA_SABOTAJE = list_create();
    iterar_cola_ready(cambiar_status_ready);

    // Por las dudas, despues se van a reiniciar
    sem_post(&ACTIVE_THREADS);
    sem_post(&TRIPULANTES_EN_COLA);
    //for(uint16_t i = 0; i < largo_cola(); i++)
    //    sem_post(&TRIPULANTES_EN_COLA);
    sem_destroy(&TRIPULANTES_EN_COLA);
    sem_init(&TRIPULANTES_EN_COLA, 0, largo_cola());
    pthread_mutex_unlock(&MUTEX_LISTA_SABOTAJE);

    // Reiniciamos los semaforos
    sem_destroy(&ACTIVE_THREADS);
    sem_init(&ACTIVE_THREADS, 0, DISCORDIADOR_CFG->GRADO_MULTITAREA);

    SABOTAJE_ACTIVO = false;
    sem_post(&BLOQUEAR_PLANIFICADOR); // Dejamos que el planificador vuelva a funcionar
}

void listener_sabotaje() {
    char* port_i_mongo_store = string_itoa(DISCORDIADOR_CFG->PUERTO_I_MONGO_STORE);
    int fd = crear_conexion(
            main_log,
            "I_MONGO_STORE",
            DISCORDIADOR_CFG->IP_I_MONGO_STORE,
            port_i_mongo_store
    );
    free(port_i_mongo_store);

    log_info(main_log, "Inicializado el listener de sabotajes.");
    send_handshake_sabotaje(fd);

    while(1) {
        t_posicion* pos_sabotaje;
        t_tarea* tarea_sabotaje = malloc(sizeof(t_tarea));
        tarea_sabotaje->nombre = strdup("Sabotaje");
        tarea_sabotaje->param = 0;
        tarea_sabotaje->duracion = DISCORDIADOR_CFG->DURACION_SABOTAJE;
        tarea_sabotaje->tipo = OTRO_T;
        op_code cop;
        recv(fd, &cop, sizeof(cop), 0);
        recv_sabotaje(fd, &pos_sabotaje); // Nos quedamos esperando a que llegue un sabotaje
        tarea_sabotaje->pos = pos_sabotaje;
        iniciar_sabotaje(tarea_sabotaje, fd);
        free_t_tarea(tarea_sabotaje);
    }
}