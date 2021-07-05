#include "../include/manejo_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;
extern t_list* segmentos_libres;
extern void* memoria_principal;
extern uint32_t memoria_disponible;
extern segmento_t* (*proximo_hueco)(uint32_t);

extern t_list* tp_patotas;
extern frame_t* tabla_frames;

extern pthread_mutex_t MUTEX_MP;
extern sem_t SEM_COMPACTACION_DONE;
extern sem_t SEM_COMPACTACION_START;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

// Varios

static uint32_t cant_paginas(uint32_t size, size_t* rem) {
    uint32_t t_pag = cfg->TAMANIO_PAGINA;
    *rem = size % t_pag;
    return (*rem) ? size/t_pag + 1 : size/t_pag;
}

bool entra_en_mp(uint32_t tamanio) {
    size_t rem;
    return cfg->SEG
        ? memoria_disponible   >= tamanio
        : cant_frames_libres() >= cant_paginas(tamanio, &rem);
}

bool entra_en_swap(uint32_t tamanio) {
    return true;
    // TODO
}

void dump_mp() {
    char* timestamp = temporal_get_string_time("%d_%m_%y--%H_%M_%S");
    char* filename = string_from_format("Dump_%s.dmp", timestamp);
    FILE* dump_file = fopen(filename, "w+");

    char* hr = string_repeat('-', 50);
    char* data = NULL;
    if (cfg->SEG) {
        char* str_segus = stringify_segus();
        data = string_from_format(
            "\n%s\nDump: %s\n%s\n%s\n\n",
            hr, timestamp, str_segus, hr
        );
        free(str_segus);
    }
    else {
        // TODO: get data paginacion a "data"
        data = strdup("paginacion!!");
    }

    fprintf(dump_file, "%s", data);

    goto die;
    die:
        fclose(dump_file);
        free(hr);
        free(data);
        free(timestamp);
        free(filename);
}

////// MANEJO MEMORIA PRINCIPAL - SEGMENTACION

bool get_structures_from_tabla_tripulante(ts_tripulante_t* tabla, TCB_t** p_tcb, PCB_t** p_pcb) {
    // Leemos TCB
    void* s_tcb = get_segmento_data(
        tabla->tcb->inicio,
        tabla->tcb->tamanio
    );
    TCB_t* tcb = deserializar_tcb(s_tcb);
    *p_tcb = tcb;

    // Leemos PCB
    void* s_pcb = get_segmento_data(tcb->dl_pcb, 8);
    PCB_t* pcb = deserializar_pcb(s_pcb);
    *p_pcb = pcb;

    free(s_tcb); free(s_pcb);
    return true;
}

bool get_structures_from_tid
(uint32_t tid, ts_tripulante_t** p_tabla_tripulante, TCB_t** p_tcb, PCB_t** p_pcb) {
    ts_tripulante_t* tabla_tripulante = list_find_by_tid_tstripulantes(tid);
    if (tabla_tripulante == NULL) return false;
    *p_tabla_tripulante = tabla_tripulante;

    // Leemos TCB
    void* s_tcb = get_segmento_data(
        tabla_tripulante->tcb->inicio,
        tabla_tripulante->tcb->tamanio
    );
    TCB_t* tcb = deserializar_tcb(s_tcb);
    *p_tcb = tcb;

    // Leemos PCB
    void* s_pcb = get_segmento_data(tcb->dl_pcb, 8);
    PCB_t* pcb = deserializar_pcb(s_pcb);
    *p_pcb = pcb;

    free(s_tcb); free(s_pcb);

    return true;
}

// Meter chorizo de bytes en MP y actualiza listas de registro.
// Si se llama a esta func es porque ya se sabe que entra
// Devuelve la Dir fisica del segmento, o INICIO_INVALIDO si explota (no deberia pasar)
uint32_t meter_segmento_en_mp(void* data, uint32_t size, tipo_segmento_t tipo) {
    segmento_t* hueco_victima = (*proximo_hueco)(size);
    if (hueco_victima == NULL) {
        if (!compactar_mp()) {
            log_error(logger, "Fallo la compactacion. QEPD.");
            return INICIO_INVALIDO;
        }

        log_warning(logger, "Compacte todos!");

        hueco_victima = (*proximo_hueco)(size);
        if (hueco_victima == NULL)
            return INICIO_INVALIDO; // no hay hueco (no deberia pasar)
    }

    uint32_t inicio = hueco_victima->inicio;
    memcpy_segmento_en_mp(hueco_victima->inicio, data, size);

    if (!meter_segmento_actualizar_hueco(hueco_victima, size)) {
        log_error(logger, "Error catastrofico metiendo segmento en MP");
        return INICIO_INVALIDO;
    }

    memoria_disponible -= size;
    if (memoria_disponible < 0) {
        // Algo dentro de mi me dijo: pone esto por las dudas
        log_error(logger, "ROMPISTE TODO, QUE HICISTE?");
        return INICIO_INVALIDO;
    }

    list_add_segus(new_segmento(tipo, 0, inicio, size)); // Al final, por si exploto todo antes

    return inicio;
}

bool eliminar_segmento_de_mp(uint32_t inicio) {
    segmento_t* segmento = list_find_by_inicio_segus(inicio);
    if (segmento == NULL) return false;

    // Nuevo hueco
    segmento_t* hueco_generado = new_segmento(0, 0, segmento->inicio, segmento->tamanio);
    segmento_t* hueco_posterior = list_find_first_by_inicio_seglib(hueco_generado->inicio+hueco_generado->tamanio);
    segmento_t* hueco_anterior = list_find_first_by_inicio_seglib(hueco_generado->inicio-1);
    if (hueco_posterior) {
        hueco_generado->tamanio += hueco_posterior->tamanio;
        hueco_posterior->tamanio = 0;
        remove_zero_sized_gap_seglib();
    }
    if (hueco_anterior) {
        hueco_generado->inicio = hueco_anterior->inicio;
        hueco_generado->tamanio += hueco_anterior->tamanio;
        hueco_anterior->tamanio = 0;
        remove_zero_sized_gap_seglib();
    }
    list_add_seglib(hueco_generado);

    // Importante
    memoria_disponible += segmento->tamanio;

    // Memset fisico
    memset_0_segmento_en_mp(segmento->inicio, segmento->tamanio);
    list_remove_by_inicio_segus(segmento->inicio);
    return true;
}

static void fix_inicio_seg_en_mp(segmento_t* seg, uint32_t inicio_destino) {
    // sangre sudor y lagrimas
    switch (seg->tipo) {
        case TCB_SEG:
        {
            ts_tripulante_t* tabla_trip = list_find_by_inicio_tcb_tstripulantes(seg->inicio);
            tabla_trip->tcb->inicio = inicio_destino;
            break;
        }
        case PCB_SEG:
        {
            // "Nosotros no buscamos eficiencia en el TP"
            ts_patota_t* tabla_patota = list_find_by_inicio_pcb_tspatotas(seg->inicio);
            tabla_patota->pcb->inicio = inicio_destino;

            t_list* tcbs = list_get_tcb_segments_segus();
            t_list_iterator* i_tcbs = list_iterator_create(tcbs);
            while (list_iterator_has_next(i_tcbs)) {
                segmento_t* seg_tcb = list_iterator_next(i_tcbs);
                void* s_tcb = get_segmento_data(seg_tcb->inicio, seg_tcb->tamanio);
                TCB_t* tcb = deserializar_tcb(s_tcb);
                free(s_tcb);

                if (tcb->dl_pcb == seg->inicio) {
                    tcb->dl_pcb = inicio_destino;
                    s_tcb = serializar_tcb(tcb);
                    memcpy_segmento_en_mp(seg_tcb->inicio, s_tcb, seg_tcb->tamanio);
                    free(s_tcb);
                }
                free(tcb);
            }
            list_iterator_destroy(i_tcbs);
            list_destroy(tcbs);
            break;
        }
        case TAREAS_SEG:
        {
            ts_patota_t* tabla_patota = list_find_by_inicio_tareas_tspatotas(seg->inicio);
            tabla_patota->tareas->inicio = inicio_destino;

            t_list* pcbs = list_get_pcb_segments_segus();
            t_list_iterator* i_pcbs = list_iterator_create(pcbs);
            while (list_iterator_has_next(i_pcbs)) {
                segmento_t* seg_pcb = list_iterator_next(i_pcbs);
                void* s_pcb = get_segmento_data(seg_pcb->inicio, seg_pcb->tamanio);
                PCB_t* pcb = deserializar_pcb(s_pcb);
                free(s_pcb);

                if (pcb->dl_tareas == seg->inicio) {
                    pcb->dl_tareas = inicio_destino;
                    s_pcb = serializar_pcb(pcb);
                    memcpy_segmento_en_mp(seg_pcb->inicio, s_pcb, seg_pcb->tamanio);
                    free(s_pcb);
                }
                free(pcb);
            }
            list_iterator_destroy(i_pcbs);
            list_destroy(pcbs);
            break;
        }
        default: break; // para tests
    }
}
static bool compactar_mp_iteracion(uint32_t i) {
    segmento_t* segmento = list_get_segus(i);
    if (segmento == NULL) return false;     // no existe!!
    if (segmento->inicio == 0) return true; // es el primero y esta pegadito al techo

    segmento_t* hueco = list_find_first_by_inicio_seglib(segmento->inicio-1);
    if (hueco == NULL) return true;         // ya esta compactado

    uint32_t inicio_original = segmento->inicio;
    uint32_t inicio_destino = hueco->inicio;

    // Guardamos varias DLs en memoria... hay que actualizarlas
    fix_inicio_seg_en_mp(segmento, inicio_destino);

    // Muevo segmento y muevo hueco
    segmento->inicio = hueco->inicio;
    list_sort_segus();

    hueco->inicio = segmento->inicio + segmento->tamanio;

    // Posible merge
    segmento_t* hueco_posterior = list_find_first_by_inicio_seglib(hueco->inicio+hueco->tamanio);
    if (hueco_posterior) {
        hueco->tamanio += hueco_posterior->tamanio;
        hueco_posterior->tamanio = 0;
        remove_zero_sized_gap_seglib();
    }

    // Movemos el segmento en la memoria fisica
    realloc_segmento_en_mp(inicio_original, inicio_destino, segmento->tamanio);
    return true;
}
bool compactar_mp() {
    if (list_is_empty_segus()) {
        return true;
    }

    log_info(logger, "Compactando memoria...");
    uint32_t segmentos = list_size_segus();
    for (int i=0; i<segmentos; i++) {
        // log_warning(logger, "Compactando segmento [%d] de %d...", i, segmentos);
        if(!compactar_mp_iteracion(i)) {
            return false;
        }
    }
    return true;
}

////// UTILS SEGMENTOS_LIBRES

static void* hueco_menor(void* h1, void* h2) {
    segmento_t* hueco1 = (segmento_t*) h1;
    segmento_t* hueco2 = (segmento_t*) h2;
    return hueco1->tamanio < hueco2->tamanio? h1 : h2;
}

// Meto un segmento nuevo a la MP, tengo menos huecos
bool meter_segmento_actualizar_hueco(segmento_t* hueco_target, uint32_t tamanio) {
    // Cosas que no deberian pasar nunca:
    if (hueco_target == NULL || (hueco_target->tamanio < tamanio)) //??
        return false;

    if (hueco_target->tamanio == tamanio) {
        hueco_target->tamanio = 0;
        remove_zero_sized_gap_seglib(); // no es lo mas eficiente/elegante, pero si lo mas facil
        return true;
    }

    hueco_target->inicio += tamanio;
    hueco_target->tamanio -= tamanio;
    return true;
}

// De todos los huecos, elige el que minimiza el espacio desaprovechado (si hay empate, el ultimo)
segmento_t* proximo_hueco_best_fit(uint32_t tamanio) {
    t_list* huecos_disponibles = list_filter_by_min_size_seglib(tamanio);

    if (list_size(huecos_disponibles) == 0) {
        list_destroy(huecos_disponibles);
        return NULL;
    }
    segmento_t* ret = (segmento_t*) list_get_minimum(huecos_disponibles, (void*) &hueco_menor);
    list_destroy(huecos_disponibles);

    return ret;
}

// De todos los huecos, elige el primero en el que entra el tamanio dado
segmento_t* proximo_hueco_first_fit(uint32_t tamanio) {
    return list_find_first_by_min_size_seglib(tamanio);
}

void compactar_segmentos_libres() {
    list_clean_seglib();
    list_add_seglib(
        new_segmento(
            0, 0,
            cfg->TAMANIO_MEMORIA-memoria_disponible,
            memoria_disponible
        )
    );
}


////// MANEJO MEMORIA PRINCIPAL - PAGINACION

static bool meter_pagina_en_mp(void* data, size_t size, uint32_t pid) {
    uint32_t inicio;
    int64_t frame_libre = primer_frame_libre_framo(pid, &inicio);
    if (frame_libre == -1) return false;
    // TODO: CONTEMPLAR MEMORIA VIRTUAL

    uint32_t nro_frame = frame_libre; // una especie de casteo porlas

    ocupar_frame_framo(nro_frame, size, pid);
    memcpy_pagina_en_frame_mp(nro_frame, inicio, data, size);

    printf(
        "Ocupe el frame %" PRIu32 " desde el inicio %" PRIu32 " con data de size %zu\n",
        nro_frame, inicio, size
    );
    return true;
}

// Dado un stream de bytes, lo mete en MP donde encuentre paginas libres
// O si la ultima del proceso esta por la mitad, empieza por ahi
#define MIN(A,B) ((A)<(B)?(A):(B))
bool append_data_to_patota_en_mp(void* data, size_t size, uint32_t pid) {
    void* buf;
    uint32_t t_pag = cfg->TAMANIO_PAGINA;

    // Data de la primera pag libre, para saber si esta por la mitad o que
    uint32_t offset;
    int64_t pag_fragmentada = primer_frame_libre_framo(pid, &offset);

    size_t rem;
    size_t size_ajustado = size - (offset ? t_pag-offset : 0);
    uint32_t n_pags = cant_paginas(size_ajustado, &rem);  // iteraciones sin offset
    if (offset) n_pags++;                                 // iteraciones ajustadas

    printf("Vou inserir um size %zu com rem %zu e offset %" PRIu32 " fazendo %" PRIu32 " iteracoes\n",
        size, rem, offset, n_pags);

    // Itera de a una pagina y las mete en MP
    for (uint32_t i=0; i<n_pags; i++) {
        size_t size_chunk = i==0
            ? MIN(t_pag - offset, size)        // Primera pagina, posible offset
            : rem && i==n_pags-1? rem : t_pag; // Otras paginas, si es la ultima es tamanio rem si hay rem

        printf("O size do chunk e: %zu\n", size_chunk);
        buf = malloc(size_chunk);
        if (i == 0)      memcpy(buf, data, size_chunk);
        else if (offset) memcpy(buf, data+(i-1)*t_pag+offset, size_chunk);
        else             memcpy(buf, data+i*t_pag, size_chunk);

        if (!meter_pagina_en_mp(buf, size_chunk, pid)) {
            free(buf);
            return false;
        }
        free(buf);
    }

    // actualizar tp_patotas

    return true;
}
