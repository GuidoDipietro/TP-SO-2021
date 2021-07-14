#include "../include/monitor_memoria.h"

extern t_log* logger; // por funciones de debug
extern t_config_mrhq* cfg;
extern t_list* segmentos_libres;
extern t_list* segmentos_usados;

extern t_list* tp_patotas;
extern frame_t* tabla_frames;

extern void* memoria_principal;

/// mutex y semaforos

pthread_mutex_t MUTEX_SEGMENTOS_LIBRES;
pthread_mutex_t MUTEX_SEGMENTOS_USADOS;
pthread_mutex_t MUTEX_FRAMO;

pthread_mutex_t MUTEX_MP;
pthread_mutex_t MUTEX_TS_PATOTAS;
pthread_mutex_t MUTEX_TS_TRIPULANTES;

pthread_mutex_t MUTEX_TP_PATOTAS;
pthread_mutex_t MUTEX_TID_PID_LOOKUP;

pthread_mutex_t MUTEX_MP_BUSY;

sem_t SEM_INICIAR_SELF_EN_PATOTA;
sem_t SEM_COMPACTACION_START;
sem_t SEM_COMPACTACION_DONE;

void iniciar_mutex() {
    pthread_mutex_init(&MUTEX_SEGMENTOS_LIBRES, NULL);
    pthread_mutex_init(&MUTEX_SEGMENTOS_USADOS, NULL);
    pthread_mutex_init(&MUTEX_FRAMO, NULL);
    pthread_mutex_init(&MUTEX_MP, NULL);
    pthread_mutex_init(&MUTEX_TS_PATOTAS, NULL);
    pthread_mutex_init(&MUTEX_TS_TRIPULANTES, NULL);
    pthread_mutex_init(&MUTEX_TP_PATOTAS, NULL);
    pthread_mutex_init(&MUTEX_TID_PID_LOOKUP, NULL);
    pthread_mutex_init(&MUTEX_MP_BUSY, NULL);

    sem_init(&SEM_INICIAR_SELF_EN_PATOTA, 0, 0);
    sem_init(&SEM_COMPACTACION_START, 0, 0);
    sem_init(&SEM_COMPACTACION_DONE, 0, 0);
}

void finalizar_mutex() {
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_LIBRES);
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_USADOS);
    pthread_mutex_destroy(&MUTEX_FRAMO);
    pthread_mutex_destroy(&MUTEX_MP);
    pthread_mutex_destroy(&MUTEX_TS_PATOTAS);
    pthread_mutex_destroy(&MUTEX_TS_TRIPULANTES);
    pthread_mutex_destroy(&MUTEX_TP_PATOTAS);
    pthread_mutex_destroy(&MUTEX_TID_PID_LOOKUP);
    pthread_mutex_destroy(&MUTEX_MP_BUSY);
    
    sem_destroy(&SEM_INICIAR_SELF_EN_PATOTA);
    sem_destroy(&SEM_COMPACTACION_DONE);
    sem_destroy(&SEM_COMPACTACION_START);
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
static bool seg_es_tcb(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tipo == TCB_SEG;
}
static bool seg_es_pcb(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tipo == PCB_SEG;
}
static bool comp_segmento_t_indice(void* s1, void* s2) {
    segmento_t* seg1 = (segmento_t*) s1;
    segmento_t* seg2 = (segmento_t*) s2;
    return seg1->inicio < seg2->inicio;
}

/// cosas que serian static pero las uso en otro lado

segmento_t* new_segmento(tipo_segmento_t tipo, uint32_t nro, uint32_t inicio, uint32_t taman) {
    segmento_t* seg = malloc(sizeof(segmento_t));
    seg->tipo = tipo;
    seg->nro_segmento = nro;
    seg->tamanio = taman;
    seg->inicio = inicio;
    return seg;
}
segmento_t* segmento_t_duplicate(segmento_t* s) {
    if (s==NULL) return NULL;
    segmento_t* seg = malloc(sizeof(segmento_t));
    seg->tipo = s->tipo;
    seg->nro_segmento = s->nro_segmento;
    seg->inicio = s->inicio;
    seg->tamanio = s->tamanio;
    return seg;
}

/// funcs

            // SEGMENTACION

void memcpy_segmento_en_mp(uint32_t inicio, void* data, uint32_t size) {
    pthread_mutex_lock(&MUTEX_MP);
    memcpy(memoria_principal+inicio, data, size); // size_t == uint32_t en la VM (chequeado)
    pthread_mutex_unlock(&MUTEX_MP);
}

void* get_segmento_data(uint32_t inicio, uint32_t size) {
    void* stream = malloc(size);
    pthread_mutex_lock(&MUTEX_MP);
    memcpy(stream, memoria_principal+inicio, size); // size_t == uint32_t en la VM (chequeado)
    pthread_mutex_unlock(&MUTEX_MP);
    return stream;
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

            // PAGINACION
// size_t == uint32_t en la VM (chequeado)

void* get_pagina_data(uint32_t nro_frame) {
    void* data = malloc(cfg->TAMANIO_PAGINA);
    pthread_mutex_lock(&MUTEX_MP);
    memcpy(data, memoria_principal+nro_frame*cfg->TAMANIO_PAGINA, cfg->TAMANIO_PAGINA);
    pthread_mutex_unlock(&MUTEX_MP);
    return data;
}

void memcpy_pagina_en_frame_mp(uint32_t nro_frame, uint32_t inicio, void* data, size_t size) {
    pthread_mutex_lock(&MUTEX_MP);
    memcpy(memoria_principal+nro_frame*cfg->TAMANIO_PAGINA+inicio, data, size);
    pthread_mutex_unlock(&MUTEX_MP);
}

void clear_frame_en_mp(uint32_t nro_frame) {
    pthread_mutex_lock(&MUTEX_MP);
    memset(memoria_principal+nro_frame*cfg->TAMANIO_PAGINA, 0, cfg->TAMANIO_PAGINA);
    pthread_mutex_unlock(&MUTEX_MP);
}

            // UTILS - BIBLIOTECAS SEGLIB | SEGUS | FRAMO

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
    uint32_t size = list_size(segmentos_usados);
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

t_list* list_get_tcb_segments_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    t_list* lista_ret = list_filter(segmentos_usados, &seg_es_tcb);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
    return lista_ret;
}

t_list* list_get_pcb_segments_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    t_list* lista_ret = list_filter(segmentos_usados, &seg_es_pcb);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
    return lista_ret;
}

static char* segmento_t_en_ram_a_string_de_dump(segmento_t* seg) {
    // Retorna string dumpleable del segmento (55 bytes sin el \0)
    char* str = NULL;
    switch (seg->tipo) {
        case PCB_SEG:
        {
            ts_patota_t* tabla = list_find_by_inicio_pcb_tspatotas(seg->inicio);
            str = string_from_format(
                "Proceso: %3" PRIu32 " Segmento: %3d Inicio: %04" PRIx32 " Tamanio: %5" PRIu32 "\n",
                tabla->pid, tabla->pcb->nro_segmento, tabla->pcb->inicio, tabla->pcb->tamanio
            );
            return str;
        }

        case TAREAS_SEG:
        {
            ts_patota_t* tabla = list_find_by_inicio_tareas_tspatotas(seg->inicio);
            str = string_from_format(
                "Proceso: %3" PRIu32 " Segmento: %3d Inicio: %04" PRIx32 " Tamanio: %5" PRIu32 "\n",
                tabla->pid, tabla->tareas->nro_segmento, tabla->tareas->inicio, tabla->tareas->tamanio
            );
            return str;
        }

        case TCB_SEG:
        {
            ts_tripulante_t* tabla = list_find_by_inicio_tcb_tstripulantes(seg->inicio);
            PCB_t* pcb; TCB_t* tcb;
            if (!get_structures_from_tid_segmentacion(tabla->tid, &tabla, &tcb, &pcb)) {
                str = strdup("Error fatal en dump segmentacion");
                free(pcb); free(tcb);
                return str;
            }
            ts_patota_t* tabla_p = list_find_by_pid_tspatotas(pcb->pid);
            str = string_from_format(
                "Proceso: %3" PRIu32 " Segmento: %3d Inicio: %04" PRIx32 " Tamanio: %5" PRIu32 "\n",
                pcb->pid, tabla->tcb->nro_segmento, tabla->tcb->inicio, tabla->tcb->tamanio
            );
            free(pcb); free(tcb);
            return str;
        }

        default:
        {
            str = strdup("Epic fail, el segmento no era de ningun tipo.\n");
            return str;
        }
    }
}
char* stringify_segus() {
    const size_t size_line = 55; // sin el \0

    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    const size_t size_str = list_size(segmentos_usados) * size_line + 1; // rows * size_row + \0
    char* str = malloc(size_str);
    memset(str, 0, size_str);
    
    t_list_iterator* i_segmentos_usados = list_iterator_create(segmentos_usados);
    for (int i = 0; list_iterator_has_next(i_segmentos_usados); i++) {
        segmento_t* seg = list_iterator_next(i_segmentos_usados);
        char* line = segmento_t_en_ram_a_string_de_dump(seg);
        memcpy(str+size_line*i, line, strlen(line)); // sin el \0
        free(line);
    }
    list_iterator_destroy(i_segmentos_usados);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);

    return str;
}

void asesinar_segus() {
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_destroy_and_destroy_elements(segmentos_usados, (void*) free);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
}

////// END SEGUS

////// UTILS FRAMES - A.K.A. FRAMO

// off_t == uint32_t en la VM (chequeado)

// Primer frame libre para proceso, puede ser uno que ya uso a medias
int64_t primer_frame_libre_framo(uint32_t pid, uint32_t* inicio) {
    int64_t primero_vacio = -1;

    pthread_mutex_lock(&MUTEX_FRAMO);
    for (uint32_t i = 0; i < cfg->CANT_PAGINAS; i++) {
        if (primero_vacio == -1 && tabla_frames[i].libre)
            primero_vacio = i;

        if (tabla_frames[i].amedias && (tabla_frames[i].pid_ocupador == pid)) {
            pthread_mutex_unlock(&MUTEX_FRAMO);

            *inicio = tabla_frames[i].inicio_hueco;
            return i;
        }
    }
    pthread_mutex_unlock(&MUTEX_FRAMO);

    *inicio = 0;
    return primero_vacio;
}

uint32_t cant_frames_libres() {
    uint32_t libres = 0;
    pthread_mutex_lock(&MUTEX_FRAMO);
    for (uint32_t i = 0; i < cfg->CANT_PAGINAS; i++) {
        if (tabla_frames[i].libre == 1)
            libres++;
    }
    pthread_mutex_unlock(&MUTEX_FRAMO);
    return libres;
}

void ocupar_frame_framo(uint32_t index, size_t size, uint32_t pid) {
    pthread_mutex_lock(&MUTEX_FRAMO);
    tabla_frames[index].pid_ocupador    = pid;
    tabla_frames[index].libre           = 0;
    tabla_frames[index].inicio_hueco   += size;
    tabla_frames[index].amedias         = tabla_frames[index].inicio_hueco < cfg->TAMANIO_PAGINA;
    pthread_mutex_unlock(&MUTEX_FRAMO);
    // Despues, si amedias == 1, ya ni miramos libre
}

void liberar_frame_framo(uint32_t index) {
    pthread_mutex_lock(&MUTEX_FRAMO);
    tabla_frames[index].bytes = 0;
    tabla_frames[index].libre = 1;
    pthread_mutex_unlock(&MUTEX_FRAMO);
}

char* stringify_tabla_frames() {
    const size_t size_line = 45; // sin el \0

    pthread_mutex_lock(&MUTEX_FRAMO);
    const size_t size_str = cfg->CANT_PAGINAS * size_line + 1; // rows * size_row + \0
    char* str = malloc(size_str);
    memset(str, 0, size_str);
    
    // Itero por cada patota
    int lines = 0;
    t_list_iterator* i_tp_patotas = list_iterator_create(tp_patotas);
    for (int i = 0; list_iterator_has_next(i_tp_patotas); i++) {
        tp_patota_t* tabla_patota = list_iterator_next(i_tp_patotas);

        // Itero por cada pagina de la patota
        t_list_iterator* i_paginas = list_iterator_create(tabla_patota->paginas);
        for (int j = 0; list_iterator_has_next(i_paginas); j++) {
            entrada_tp_t* pagina = list_iterator_next(i_paginas);
            frame_t frame = tabla_frames[pagina->nro_frame];
            char* line = string_from_format(
                "Marco: %3d Libre: %d Proceso: %3d Pagina: %3d\n",
                pagina->nro_frame, frame.libre, frame.pid_ocupador, pagina->nro_pagina
            );
            memcpy(str+size_line*lines, line, size_line); // sin el \0
            lines++;
            free(line);
        }
        list_iterator_destroy(i_paginas);
    }
    list_iterator_destroy(i_tp_patotas);
    pthread_mutex_unlock(&MUTEX_FRAMO);

    return str;
}

////// END FRAMO

/// debug

static bool log = false;

void print_framo(bool ynlog) {
    ynlog ? log_info(logger, "--------- FRAMES ---------\n")
          : printf("\n\n--------- FRAMES ---------\n");
    for (uint32_t i=0; i < cfg->CANT_PAGINAS; i++) {
        ynlog?
        log_info(logger,
            "%s%2d%s",
            i&&i%8==0?i%64?"|":"\n":"",
            tabla_frames[i].libre? 0 : tabla_frames[i].pid_ocupador,
            !tabla_frames[i].libre && tabla_frames[i].amedias?".":" "
        )   :
        printf(
            "%s%2d%s",
            i&&i%8==0?i%64?"|":"\n":"",
            tabla_frames[i].libre? 0 : tabla_frames[i].pid_ocupador,
            !tabla_frames[i].libre && tabla_frames[i].amedias?".":" "
        );
    }
    ynlog ? log_info(logger, "\n------------------------------------\n\n")
        : printf("\n------------------------------------\n\n");
}
void print_segmento_t(void* s) {
    segmento_t* seg = (segmento_t*) s;
    log ? log_info(
        logger,
        "(%d) #%" PRIu32 " -- INICIO: %5" PRIu32 " | TAMAN: %5" PRIu32 "\n",
        seg->tipo, seg->nro_segmento, seg->inicio, seg->tamanio
    )   : printf(
        "(%d) #%" PRIu32 " -- INICIO: %5" PRIu32 " | TAMAN: %5" PRIu32 "\n",
        seg->tipo, seg->nro_segmento, seg->inicio, seg->tamanio
    );
}
void print_seglib(bool ynlog) {
    log = ynlog;
    ynlog ? log_info(logger, "------ HUECOS LIBRES ------\n")
          : printf("\n\n------ HUECOS LIBRES ------\n");
    pthread_mutex_lock(&MUTEX_SEGMENTOS_LIBRES);
    list_iterate(segmentos_libres, &print_segmento_t);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_LIBRES);
    ynlog ? log_info(logger, "---------------------------\n\n")
          : printf("---------------------------\n\n");
}
void print_segus(bool ynlog) {
    log = ynlog;
    ynlog ? log_info(logger, "------ SEGMENTOS USADOS ------\n")
          : printf("\n\n------ SEGMENTOS USADOS ------\n");
    pthread_mutex_lock(&MUTEX_SEGMENTOS_USADOS);
    list_iterate(segmentos_usados, &print_segmento_t);
    pthread_mutex_unlock(&MUTEX_SEGMENTOS_USADOS);
    ynlog ? log_info(logger, "------------------------------\n\n")
          : printf("------------------------------\n\n");
}
