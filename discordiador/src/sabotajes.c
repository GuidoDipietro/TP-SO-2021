#include "../include/sabotajes.h"

bool SABOTAJE_ACTIVO = false;

t_running_thread* encargado;

static void bloquear_hilo(void* p) {
    t_running_thread* t = (t_running_thread*) p;
    t->blocked = true;
}

static bool sort_by_tid(void* p1, void* p2) {
    // Perdon, de verdad
    return (((t_running_thread*) p1)->t)->tid > (((t_running_thread*) p2)->t)->tid;
}

static void cambiar_status(void* p) {
    (((t_running_thread*) p)->t)->status = BLOCKEDSAB;
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

// Esto es Haskell o C? Nadie lo sabe
void iniciar_sabotaje(int signum) {
    // Al estar todo pausado no deberiamos de preocuparnos por condiciones de carrera
    log_info(main_log, "Sabotaje detectado. Comenzando bloqueo por sabotaje.");
    SABOTAJE_ACTIVO = true; // Iniciamos todas las rutinas de sabotaje
    iterar_lista_hilos(bloquear_hilo); // Pausamos todos los hilos que esten corriendo
    list_sort(LISTA_HILOS, sort_by_tid); // Ordenamos la lista de hilos
    list_sort(COLA_TRIPULANTES->elements, sort_by_tid); // Ordenamos la cola de tripulantes

    // Bloqueando la LISTA_SABOTAJE nos aseguramos que los tripulantes que vuelven de IO entren ultimos
    pthread_mutex_lock(&MUTEX_LISTA_SABOTAJE);
    list_add_all(LISTA_SABOTAJE, LISTA_HILOS);
    list_clean(LISTA_HILOS);

    list_add_all(LISTA_SABOTAJE, COLA_TRIPULANTES->elements);
    queue_clean(COLA_TRIPULANTES);
    list_iterate(LISTA_SABOTAJE, cambiar_status);

    log_info(main_log, "Bloqueo por sabotaje finalizado.");

    encargado = list_get_minimum(LISTA_SABOTAJE, comparar_cercania_a_sabotaje);

    pthread_mutex_unlock(&MUTEX_LISTA_SABOTAJE);

    printf("\n\nSabotaje iniciado\n\n");
}

void finalizar_sabotaje(int signum) {
    SABOTAJE_ACTIVO = false;
    printf("\n\nSabotaje finalizado\n\n");
}

void set_signal_handlers() {
    signal(SIG_SABOTAJE, iniciar_sabotaje);
    signal(SIG_FIN_SABOTAJE,  finalizar_sabotaje);
}