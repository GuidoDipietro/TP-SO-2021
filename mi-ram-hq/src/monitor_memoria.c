#include "../include/monitor_memoria.h"

extern t_config_mrhq* cfg;

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
static uint32_t inicio_static = 0;
static uint32_t tamanio_acumulado = 0;

static bool seg_entra_en_hueco(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tamanio >= tamanio_static;
}
static bool seg_contiene_inicio(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return inicio_static < (seg->inicio + seg->tamanio); //safety syntax
}
static bool seg_es_nulo(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tamanio == 0;
}
static void add_tamanio_acumulado(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    tamanio_acumulado += seg->tamanio;
}

/// cosas que serian static pero las uso en otro lado

segmento_t* new_segmento(uint32_t inicio, uint32_t taman) {
    segmento_t* seg = malloc(sizeof(segmento_t));
    seg->tamanio = taman;
    seg->inicio = inicio;
    return seg;
}
segmento_t* segmento_t_duplicate(segmento_t* s) {
    if (s==NULL) return NULL;
    segmento_t* seg = malloc(sizeof(segmento_t));
    seg->inicio = s->inicio;
    seg->tamanio = s->tamanio;
    return seg;
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

segmento_t* list_find_first_by_inicio_seglib(uint32_t inicio) {
    inicio_static = inicio;

    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    segmento_t* ret = (segmento_t*) list_find(segmentos_libres, &seg_contiene_inicio);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);

    return ret;
}

segmento_t* list_add_all_holes_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    tamanio_acumulado = 0;
    list_iterate(segmentos_libres, add_tamanio_acumulado);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);

    return new_segmento(cfg->TAMANIO_MEMORIA - tamanio_acumulado, tamanio_acumulado);
    // esta func antes era re linda con un fold pero tenia un leak
    // incorregible por como son las commons :( maldito ranieri
}

void list_clean_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_clean_and_destroy_elements(segmentos_libres, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}

void remove_zero_sized_gap_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_remove_and_destroy_by_condition(segmentos_libres, &seg_es_nulo, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}

void asesinar_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_destroy_and_destroy_elements(segmentos_libres, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
}

/// debug

static void print_segmento_t(void* s) {
    segmento_t* seg = (segmento_t*) s;
    printf("INICIO: %5d | TAMAN: %5d\n", seg->inicio, seg->tamanio);
}
void print_seglib() {
    puts("\n\n------ HUECOS LIBRES ------\n");
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_iterate(segmentos_libres, &print_segmento_t);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
    puts("------------------------------\n\n");
}