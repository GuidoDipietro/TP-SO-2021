#include "../include/monitor_memoria.h"

/// mutex

pthread_mutex_t MUTEX_SEGMENTOS_LIBRES;

void iniciar_mutex() {
    pthread_mutex_init(&MUTEX_SEGMENTOS_LIBRES, NULL);
}

void finalizar_mutex() {
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_LIBRES);
}

/// statics

static uint32_t tamanio_static = 0;

static bool seg_entra_en_hueco(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tamanio >= tamanio_static;
}

static void* merge_segmento_t(void* s1, void* s2) { // union de 2 segmentos por tamanio
    segmento_t* seg1 = (segmento_t*) s1;
    segmento_t* seg2 = (segmento_t*) s2;
    segmento_t* merge = malloc(sizeof(segmento_t));
    merge->tamanio = seg1->tamanio + seg2->tamanio;
    merge->inicio = 0; // para que tenga algun valor no-basura

    return (void*) merge;
}

/// funcs

void list_add_seglib(segmento_t* seg) {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_add(segmentos_libres, (void*) seg);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}

t_list* list_filter_by_min_size_seglib(uint32_t min_size) {
    tamanio_static = min_size;

    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    t_list* ret = list_filter(segmentos_libres, &seg_entra_en_hueco);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);

    return ret;
}

segmento_t* list_find_first_by_min_size_seglib(uint32_t min_size) {
    tamanio_static = min_size;

    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    segmento_t* ret = (segmento_t*) list_find(segmentos_libres, &seg_entra_en_hueco);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);

    return ret;
}

segmento_t* list_add_all_holes_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    segmento_t* ret = (segmento_t*) list_fold1(segmentos_libres, &merge_segmento_t);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);

    return ret;
}

void list_clean_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_clean_and_destroy_elements(segmentos_libres, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}

void asesinar_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_destroy_and_destroy_elements(segmentos_libres, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}

/// debug

static void print_segmento_t(segmento_t* seg) {
    printf("TAMAN: %5d | INICIO: %5d\n", seg->tamanio, seg->inicio);
}
void print_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}