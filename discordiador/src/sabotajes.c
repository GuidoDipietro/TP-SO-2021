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
    (((t_running_thread*) p)->t)->status = BLOCKEDSAB;
}

static void cambiar_status_ready(void* p) {
    (((t_running_thread*) p)->t)->status = READY;
}

static t_posicion pos_sabotaje = {6, 2}; // Temporal. Hasta que este implementado el i-Mongo-Store

// no existe una libreria estandar para numeros de punto flotante :(
static float calcular_distancia_a_sabotaje(void* p) {
    t_posicion* pos = (((t_running_thread*) p)->t)->pos;
    uint16_t difx = abs(pos_sabotaje.x - pos->x);
    uint16_t dify = abs(pos_sabotaje.y - pos->y);
    return sqrt(difx*difx+dify*dify);
}

static void* comparar_cercania_a_sabotaje(void* p1, void* p2) {
    float d1 = calcular_distancia_a_sabotaje(p1);
    float d2 = calcular_distancia_a_sabotaje(p2);
    return d1 < d2 ? p1 : p2;
}

static bool comparator_remover_encargado(void* p) {
    return p == encargado;
}

static t_tarea* sabotaje_simulado() {
    t_tarea* t = malloc(sizeof(t_tarea));
    t->nombre = "Sabotaje epico";
    t->tipo = OTRO_T;
    t->duracion = DISCORDIADOR_CFG->DURACION_SABOTAJE;
    t_posicion* pos = malloc(sizeof(t_posicion));
    pos->x = 2;
    pos->y = 7;
    t->pos = pos;
    t->param = 0;
    return t;
}

// Esto es Haskell o C? Nadie lo sabe
void iniciar_sabotaje(int signum) {
    // Al estar todo pausado no deberiamos de preocuparnos por condiciones de carrera
    log_info(main_log, "Sabotaje detectado. Comenzando bloqueo por sabotaje.");
    SABOTAJE_ACTIVO = true; // Iniciamos todas las rutinas de sabotaje

    iterar_lista_hilos(bloquear_hilo); // Pausamos todos los hilos que esten corriendo

    //for(uint8_t i = 0; i < largo_lista_hilos(); i++)
    //    sem_wait(&TRIPULANTE_LISTA_HILOS_PAUSADO); // Esperamos a que todos los hilos avisen que ya frenaron
    printf("\n%d\n", largo_lista_hilos());

    list_sort(LISTA_HILOS, sort_by_tid); // Ordenamos la lista de hilos
    list_sort(COLA_TRIPULANTES->elements, sort_by_tid); // Ordenamos la cola de tripulantes

    for(uint16_t i = 0; i < largo_cola(); i++) // Marcamos que no hay tripulantes en cola
        sem_wait(&TRIPULANTES_EN_COLA);

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

    encargado = list_get_minimum(LISTA_SABOTAJE, comparar_cercania_a_sabotaje);
    list_remove_by_condition(LISTA_SABOTAJE, comparator_remover_encargado); // Sacamos al encargado de esta lista

    pthread_mutex_unlock(&MUTEX_LISTA_SABOTAJE); // Aca ya podemos liberar la LISTA_SABOTAJE

    log_info(main_log, "El tripulante %d es el encargado de resolver el sabotaje", (encargado->t)->tid);

    t_tarea* anterior = (encargado->t)->tarea;
    (encargado->t)->tarea = sabotaje_simulado();
    (encargado->t)->status = EXEC;
    monitor_add_lista_hilos(encargado);

    while(((encargado->t)->tarea)->duracion) {
        ciclo();
        correr_tarea(encargado);
    }

    log_info(main_log, "Sabotaje resuelto");

    //free_t_tarea((encargado->t)->tarea);
    // Volvemos a meter al encargado en la cola de bloqueados
    remover_lista_hilos(((encargado)->t)->tid);
    (encargado->t)->tarea = anterior;
    (encargado->t)->status = BLOCKEDSAB;
    list_add(LISTA_SABOTAJE, encargado);


    finalizar_sabotaje();
}

void finalizar_sabotaje() {
    pthread_mutex_lock(&MUTEX_LISTA_SABOTAJE);
    list_destroy(COLA_TRIPULANTES->elements);
    COLA_TRIPULANTES->elements = LISTA_SABOTAJE;
    LISTA_SABOTAJE = list_create();
    iterar_cola_ready(cambiar_status_ready);
    SABOTAJE_ACTIVO = false;

    for(uint16_t i = 0; i < largo_cola(); i++)
        sem_post(&TRIPULANTES_EN_COLA);

    pthread_mutex_unlock(&MUTEX_LISTA_SABOTAJE);
    sem_post(&BLOQUEAR_PLANIFICADOR); // Dejamos que el planificador vuelva a funcionar

}

void set_signal_handlers() {
    signal(SIG_SABOTAJE, iniciar_sabotaje);
}