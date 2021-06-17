#include "../include/monitor_memoria.h"

extern t_config_mrhq* cfg;
extern t_list* segmentos_libres;
extern t_list* segmentos_usados;
extern void* memoria_principal;

/// mutex

pthread_mutex_t MUTEX_SEGMENTOS_LIBRES;
pthread_mutex_t MUTEX_SEGMENTOS_USADOS;
pthread_mutex_t MUTEX_MP;
pthread_mutex_t MUTEX_TS_PATOTAS;
pthread_mutex_t MUTEX_TS_TRIPULANTES;

void iniciar_mutex() {
    pthread_mutex_init(&MUTEX_SEGMENTOS_LIBRES, NULL);
    pthread_mutex_init(&MUTEX_SEGMENTOS_USADOS, NULL);
    pthread_mutex_init(&MUTEX_MP, NULL);
    pthread_mutex_init(&MUTEX_TS_PATOTAS, NULL);
    pthread_mutex_init(&MUTEX_TS_TRIPULANTES, NULL);
}

void finalizar_mutex() {
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_LIBRES);
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_USADOS);
    pthread_mutex_destroy(&MUTEX_MP);
    pthread_mutex_destroy(&MUTEX_TS_PATOTAS);
    pthread_mutex_destroy(&MUTEX_TS_TRIPULANTES);
}

/// statics

static uint32_t tamanio_static = 0;
static uint32_t inicio_static = 0;

static bool seg_entra_en_hueco(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tamanio >= tamanio_static;
}
static bool seg_contiene_inicio(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return (inicio_static >= seg->inicio) && (inicio_static < (seg->inicio+seg->tamanio)); //safety syntax
}
static bool seg_empieza_en(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->inicio == inicio_static;
}
static bool seg_es_nulo(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tamanio == 0;
}
static bool comp_segmento_t_indice(void* s1, void* s2) {
    segmento_t* seg1 = (segmento_t*) s1;
    segmento_t* seg2 = (segmento_t*) s2;
    return seg1->inicio < seg2->inicio;
}

/// cosas que serian static pero las uso en otro lado

segmento_t* new_segmento(uint8_t n, uint32_t inicio, uint32_t taman) {
    segmento_t* seg = malloc(sizeof(segmento_t));
    seg->nro_segmento = n;
    seg->tamanio = taman;
    seg->inicio = inicio;
    return seg;
}
segmento_t* segmento_t_duplicate(segmento_t* s) {
    if (s==NULL) return NULL;
    segmento_t* seg = malloc(sizeof(segmento_t));
    seg->nro_segmento = s->nro_segmento;
    seg->inicio = s->inicio;
    seg->tamanio = s->tamanio;
    return seg;
}

/// funcs

void dump_mp() {
    char* timestamp = temporal_get_string_time("%d_%m_%y--%H_%M_%S");
    char* filename = calloc(9 + strlen(timestamp) + 1, 1);
    snprintf(filename, 9 + strlen(timestamp) + 1, "Dump_%s.dmp", timestamp);
    FILE* dump_file = fopen(filename, "w+");

    char* data = malloc(13);
    // TO DO: this
    if (strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION") == 0) {
        // get data segmentacion a "data"
        snprintf(data, 13, "segmentacion");
    }
    else if (strcmp(cfg->ESQUEMA_MEMORIA, "PAGINACION") == 0) {
        // get data paginacion a "data"
        snprintf(data, 13, "paginacion!!");
    }
    else goto die;

    fprintf(dump_file, "%s", data);

    die:
        fclose(dump_file);
        free(data);
        free(timestamp);
        free(filename);
}

void memcpy_segmento_en_mp(uint32_t inicio, void* data, uint32_t size) {
    pthread_mutex_lock(&MUTEX_MP);
    memcpy(memoria_principal+inicio, data, size); // size_t == uint32_t en la VM (chequeado)
    pthread_mutex_unlock(&MUTEX_MP);
}

void memset_0_segmento_en_mp(uint32_t inicio, uint32_t tamanio) {
    pthread_mutex_lock(&MUTEX_MP);
    memset(memoria_principal+inicio, 0, tamanio);
    pthread_mutex_unlock(&MUTEX_MP);
}

void realloc_segmento_en_mp(uint32_t inicio, uint32_t destino, uint32_t tamanio) {
    // No es un realloc como la lib de stdlib.h, es mas bien mover un cacho de memoria a otro lado
    void* data = malloc(tamanio);

    pthread_mutex_lock(&MUTEX_MP);
    memcpy(data, memoria_principal+inicio, tamanio);
    memset(memoria_principal+inicio, 0, tamanio);
    memcpy(memoria_principal+destino, data, tamanio);
    pthread_mutex_unlock(&MUTEX_MP);

    free(data);
}

////// UTILS SEGMENTOS_LIBRES A.K.A. SEGLIB

uint32_t list_size_seglib() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    uint32_t size = list_size(segmentos_libres);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
    return size;
}

void list_add_seglib(segmento_t* seg) {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_add_sorted(segmentos_libres, (void*) seg, &comp_segmento_t_indice);
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

segmento_t* list_get_seglib(uint32_t indice) {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    segmento_t* seg = list_get(segmentos_libres, indice);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);

    return seg;
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

////// END SEGLIB

////// UTILS SEGMENTOS USADOS - A.K.A. SEGUS

bool list_is_empty_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    bool sino = list_is_empty(segmentos_usados);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
    return sino;
}

void list_add_segus(segmento_t* seg) {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_add_sorted(segmentos_usados, (void*) seg, &comp_segmento_t_indice);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
}

void list_sort_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_sort(segmentos_usados, &comp_segmento_t_indice);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
}

void list_remove_by_inicio_segus(uint32_t inicio) {
    inicio_static = inicio;

    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_remove_and_destroy_by_condition(segmentos_usados, (void*) &seg_empieza_en, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
}

uint32_t list_size_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    uint32_t size = list_size(segmentos_libres);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
    return size;
}

segmento_t* list_find_by_inicio_segus(uint32_t inicio) {
    inicio_static = inicio;

    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    segmento_t* ret = list_find(segmentos_usados, (void*) &seg_empieza_en);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);

    return ret;
}

segmento_t* list_get_segus(uint32_t indice) {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    segmento_t* seg = list_get(segmentos_usados, indice);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);

    return seg;
}

void asesinar_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_destroy_and_destroy_elements(segmentos_usados, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
}

////// END SEGUS

/// debug

void print_segmento_t(void* s) {
    segmento_t* seg = (segmento_t*) s;
    printf(
        "#%" PRIu32 " -- INICIO: %5" PRIu32 " | TAMAN: %5" PRIu32 "\n",
        seg->nro_segmento, seg->inicio, seg->tamanio
    );
}
void print_seglib() {
    puts("\n\n------ HUECOS LIBRES ------\n");
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_iterate(segmentos_libres, &print_segmento_t);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
    puts("---------------------------\n\n");
}
void print_segus() {
    puts("\n\n------ SEGMENTOS USADOS ------\n");
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_iterate(segmentos_usados, &print_segmento_t);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
    puts("------------------------------\n\n");
}
