#include "../include/monitor_tablas.h"

extern t_log* logger;
extern t_config_mrhq* cfg;

extern pthread_mutex_t MUTEX_TS_PATOTAS;
extern pthread_mutex_t MUTEX_TS_TRIPULANTES;
extern pthread_mutex_t MUTEX_TP_PATOTAS;
extern pthread_mutex_t MUTEX_TID_PID_LOOKUP;

extern t_list* ts_patotas;
extern t_list* ts_tripulantes;

extern t_list* tp_patotas;
extern t_list* tid_pid_lookup;
extern frame_swap_t* tabla_frames_swap;
extern uint32_t global_TUR;

static uint32_t static_pid;
static uint32_t static_tid;
static uint32_t static_inicio;
static uint32_t static_nro_pag;
static uint32_t static_nro_frame;

extern void* memoria_principal; // solo por un print
extern void* area_swap;         // solo por un print

/// TS PATOTAS

// romlfoa estas dos antes era una sola funcion y destrozaba todos los structs XD
static bool ts_patota_has_pid(void* x) {
    ts_patota_t* elem = x;
    return elem->pid == static_pid;
}
static bool tp_patota_has_pid(void* x) {
    tp_patota_t* elem = x;
    return elem->pid == static_pid;
}

static bool has_tid(void* x) {
    tid_pid_lookup_t* elem = x;
    return elem->tid == static_tid;
}

static bool has_nro_pag(void* x) {
    entrada_tp_t* elem = x;
    return elem->nro_pagina == static_nro_pag;
}

static bool has_nro_frame(void* x) {
    entrada_tp_t* elem = x;
    return (elem->nro_frame == static_nro_frame) && (elem->bit_P==1);
}

static bool has_nro_frame_swap(void* x) {
    entrada_tp_t* elem = x;
    return (elem->nro_frame == static_nro_frame) && (elem->bit_P==0);
}

static bool ts_patota_t_pcb_has_inicio(void* x) {
    ts_patota_t* elem = x;
    return elem->pcb->inicio == static_inicio;
}

static bool ts_patota_t_tareas_has_inicio(void* x) {
    ts_patota_t* elem = x;
    return elem->tareas->inicio == static_inicio;
}

void list_add_tspatotas(ts_patota_t* elem) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_add(ts_patotas, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

void list_delete_by_pid_tspatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_remove_and_destroy_by_condition(ts_patotas, &ts_patota_has_pid, &free_ts_patota_t);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

// Recupera entrada de la tabla y suma 1 a tripulantes inicializados
ts_patota_t* list_find_by_pid_plus_plus_tspatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    static_pid = pid;
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_has_pid);
    if (elem)
        elem->tripulantes_inicializados = elem->tripulantes_inicializados + 1;
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

ts_patota_t* list_find_by_pid_tspatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    static_pid = pid;
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_has_pid);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

ts_patota_t* list_find_by_inicio_pcb_tspatotas(uint32_t inicio) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    static_inicio = inicio;
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_t_pcb_has_inicio);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

ts_patota_t* list_find_by_inicio_tareas_tspatotas(uint32_t inicio) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    static_inicio = inicio;
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_t_tareas_has_inicio);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

void asesinar_tspatotas() {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_destroy_and_destroy_elements(ts_patotas, &free_ts_patota_t);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

/// TS TRIPULANTES

static bool ts_tripulante_t_has_tid(void* x) {
    ts_tripulante_t* elem = (ts_tripulante_t*) x;
    return elem->tid == static_tid;
}
static bool ts_tripulante_t_tcb_has_inicio(void* x) {
    ts_tripulante_t* elem = (ts_tripulante_t*) x;
    return elem->tcb->inicio == static_inicio;
}

// Cuando expulsan un tripulante hay que actualizar los nros de segmento de los demas
void list_update_nro_seg_tcb_by_pid_tstripulantes(uint32_t tid_eliminado, uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    t_list_iterator* i_ts_tripulantes = list_iterator_create(ts_tripulantes);
    while (list_iterator_has_next(i_ts_tripulantes)) {
        ts_tripulante_t* tabla = list_iterator_next(i_ts_tripulantes);
        TCB_t* tcb; PCB_t* pcb;
        if (get_structures_from_tabla_tripulante(tabla, &tcb, &pcb)) {
            if (pcb->pid == pid && tcb->tid > tid_eliminado)
                tabla->tcb->nro_segmento--;
            free(tcb); free(pcb);
        }
    }
    list_iterator_destroy(i_ts_tripulantes);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
}

ts_tripulante_t* list_find_by_tid_tstripulantes(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    static_tid = tid;
    ts_tripulante_t* elem = list_find(ts_tripulantes, &ts_tripulante_t_has_tid);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
    return elem;
}

ts_tripulante_t* list_find_by_inicio_tcb_tstripulantes(uint32_t inicio) {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    static_inicio = inicio;
    ts_tripulante_t* elem = list_find(ts_tripulantes, &ts_tripulante_t_tcb_has_inicio);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
    return elem;
}

void list_delete_by_tid_tstripulantes(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    list_remove_and_destroy_by_condition(ts_tripulantes, &ts_tripulante_t_has_tid, &free_ts_tripulante_t);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
}

void list_add_tstripulantes(ts_tripulante_t* elem) {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    list_add(ts_tripulantes, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
}

void asesinar_tstripulantes() {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    list_destroy_and_destroy_elements(ts_tripulantes, &free_ts_tripulante_t);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
}

/// TP PATOTAS

void list_add_tppatotas(tp_patota_t* elem) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    list_add(tp_patotas, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
}

tp_patota_t* list_find_by_pid_plus_plus_tppatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    tp_patota_t* elem = list_find(tp_patotas, &tp_patota_has_pid);
    if (elem)
        elem->tripulantes_inicializados = elem->tripulantes_inicializados + 1;
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
    return elem;
}

tp_patota_t* list_find_by_pid_tppatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    tp_patota_t* elem = list_find(tp_patotas, &tp_patota_has_pid);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
    return elem;
}

tp_patota_t* list_remove_by_pid_tppatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    tp_patota_t* elem = list_remove_by_condition(tp_patotas, &tp_patota_has_pid);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
    return elem;
}

// Agrega una nueva entrada a la tabla de paginas, cuando se ocupa un nuevo frame, si estaba vacio
// (significa que se cargo una nueva pagina en memoria, total o parcialmente, ante un INICIAR_algo)
// Tambien actualiza cantidad total de paginas de la patota
// Contempla el caso en el que una pagina arranque cargada en SWAP
void list_add_page_frame_tppatotas(uint32_t pid, uint32_t nro_frame, size_t size, bool presente) {
    /*log_info(logger, "list_add_page...: pid/nro_frame/size %" PRIu32 "/%" PRIu32 "/%" PRIu32 " (P%d)",
        pid, nro_frame, size, presente
    );*/
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;

    tp_patota_t* res = list_find(tp_patotas, &tp_patota_has_pid);

    static_nro_frame = nro_frame;
    entrada_tp_t* e_pagina = presente
        ? list_find(res->paginas, &has_nro_frame)
        : list_find(res->paginas, &has_nro_frame_swap);
    if (e_pagina == NULL) {
        // Nuevo frame, nueva pagina!
        //log_info(logger, "Nuevo frame (%" PRIu32 ") (P%d)", nro_frame, presente);
        entrada_tp_t* e_pagina_new = malloc(sizeof(entrada_tp_t));
        e_pagina_new->nro_pagina = res->pages;
        e_pagina_new->nro_frame = nro_frame;

        if (cfg->LRU)
            e_pagina_new->TUR = global_TUR++;
        else
            e_pagina_new->bit_U = 1;
        
        e_pagina_new->bit_P = presente;

        list_add(res->paginas, (void*) e_pagina_new);
        /*log_info(logger, "Agregada esta entrada_tp_t*: %" PRIu32 "|%" PRIu32 "|%d",
            e_pagina_new->nro_pagina, e_pagina_new->nro_frame, e_pagina_new->bit_P
        );*/

        if (!presente) {
            tabla_frames_swap[nro_frame].pid = pid;
            tabla_frames_swap[nro_frame].nro_pagina = e_pagina_new->nro_pagina;
            tabla_frames_swap[nro_frame].inicio += size;
        }

        res->pages++;
    }
    else {
        // No nuevo frame, esta en SWAP
        //log_info(logger, "Not nuevo frame (%" PRIu32 ") (P%d)", nro_frame, presente);
        if (!presente) tabla_frames_swap[nro_frame].inicio += size;
    }
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
}

// Actualiza nro de frame en la que esta una pag. data de una patota dada
// se asume que existe la pagina indicada
void list_update_page_frame_tppatotas(uint32_t pid, uint32_t nro_pag, uint32_t nro_frame) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    static_nro_pag = nro_pag;

    tp_patota_t* res = list_find(tp_patotas, &tp_patota_has_pid);
    entrada_tp_t* e_pagina = list_find(res->paginas, &has_nro_pag);
    e_pagina->nro_frame = nro_frame;
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
}

// Actualiza cant. de paginas totales que tiene una patota
// DEPRECATED (creo) xd!
void list_update_n_of_pages_tppatotas(uint32_t n_pags, uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    tp_patota_t* res = list_find(tp_patotas, &tp_patota_has_pid);
    res->pages += n_pags;
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
}

void list_indicar_pagina_en_frame_tppatotas(uint32_t pid, uint32_t nro_pagina, uint32_t nuevo_frame) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    static_nro_pag = nro_pagina;
    tp_patota_t* res = list_find(tp_patotas, &tp_patota_has_pid);
    entrada_tp_t* pagina = list_find(res->paginas, &has_nro_pag);

    pagina->bit_P = 1;
    pagina->nro_frame = nuevo_frame;
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
}

uint32_t list_get_frame_of_page_tppatotas(uint32_t pid, uint32_t page) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    static_nro_pag = page;

    tp_patota_t* res = list_find(tp_patotas, &tp_patota_has_pid);
    entrada_tp_t* pag = list_find(res->paginas, &has_nro_pag);
    
    uint32_t nro_frame = pag->nro_frame;
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
    return nro_frame;
}

uint32_t list_get_n_of_pages_tppatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    static_pid = pid;
    tp_patota_t* res = list_find(tp_patotas, &tp_patota_has_pid);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
    return res->pages;
}

void asesinar_tppatotas() {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    list_destroy_and_destroy_elements(tp_patotas, &free_tp_patota_t);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
}

/// TID PID LOOKUP

uint32_t pid_of_tid(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_TID_PID_LOOKUP);
    static_tid = tid;
    tid_pid_lookup_t* elem = list_find(tid_pid_lookup, &has_tid);
    pthread_mutex_unlock(&MUTEX_TID_PID_LOOKUP);

    if (elem == NULL) return 0xFFFF;
    return elem->pid;
}

tid_pid_lookup_t* list_tid_pid_lookup_find_by_tid(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_TID_PID_LOOKUP);
    static_tid = tid;
    tid_pid_lookup_t* elem = list_find(tid_pid_lookup, &has_tid);
    pthread_mutex_unlock(&MUTEX_TID_PID_LOOKUP);
    return elem;
}

void list_add_tid_pid_lookup(tid_pid_lookup_t* elem) {
    pthread_mutex_lock(&MUTEX_TID_PID_LOOKUP);
    list_add(tid_pid_lookup, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TID_PID_LOOKUP);
}

void list_tid_pid_lookup_remove_by_tid(uint32_t tid) {
    pthread_mutex_lock(&MUTEX_TID_PID_LOOKUP);
    static_tid = tid;
    list_remove_and_destroy_by_condition(tid_pid_lookup, &has_tid, (void*) free);
    pthread_mutex_unlock(&MUTEX_TID_PID_LOOKUP);
}

void asesinar_tid_pid_lookup() {
    pthread_mutex_lock(&MUTEX_TID_PID_LOOKUP);
    list_destroy_and_destroy_elements(tid_pid_lookup, (void*) free);
    pthread_mutex_unlock(&MUTEX_TID_PID_LOOKUP);
}

/// DEBUG

static bool ynlog;
static void print_t_posicion1(void* x) {
    t_posicion* pos = (t_posicion*) x;
    ynlog   ? log_info(logger, "%" PRIu32 "::%" PRIu32 "\n", pos->x, pos->y)
            : printf("%" PRIu32 "::%" PRIu32 "\n", pos->x, pos->y);
}
static void print_tspatota(void* x) {
    ts_patota_t* elem = (ts_patota_t*) x;
    if (ynlog) {
        log_info(
            logger,
            "PID: %" PRIu32 " | PCB: %" PRIu32 ":%" PRIu32 " | TAREAS: %" PRIu32 ":%" PRIu32 "\n",
            elem->pid, elem->pcb->inicio, elem->pcb->tamanio, elem->tareas->inicio, elem->tareas->tamanio
        );
        list_iterate(elem->posiciones, &print_t_posicion1);
    }
    else {
        printf(
            "PID: %" PRIu32 " | PCB: %" PRIu32 ":%" PRIu32 " | TAREAS: %" PRIu32 ":%" PRIu32 "\n",
            elem->pid, elem->pcb->inicio, elem->pcb->tamanio, elem->tareas->inicio, elem->tareas->tamanio
        );
        list_iterate(elem->posiciones, &print_t_posicion1);
    }
}
void print_tspatotas(bool log) {
    ynlog = log;
    log ? log_info(logger, "\n\n------ TS PATOTAS ------")
        : printf("\n\n------ TS PATOTAS ------\n");
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_iterate(ts_patotas, &print_tspatota);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    log ? log_info(logger, "\n\n------------------------\n\n")
        :   printf("\n\n------------------------\n\n");
}

static void print_tstripulante(void* x) {
    ts_tripulante_t* elem = (ts_tripulante_t*) x;
    if (ynlog) {
        log_info(
            logger,
            "TID: %" PRIu32 " | INICIO: %" PRIu32 " | TAMANIO: %" PRIu32 "\n",
            elem->tid, elem->tcb->inicio, elem->tcb->tamanio
        );
    }
    else {
        printf(
            "TID: %" PRIu32 " | INICIO: %" PRIu32 " | TAMANIO: %" PRIu32 "\n",
            elem->tid, elem->tcb->inicio, elem->tcb->tamanio
        );
    }
}
void print_tstripulantes(bool log) {
    ynlog = log;
    log ? log_info(logger, "\n\n------ TS TRIPULANTES ------")
        : printf("\n\n------ TS TRIPULANTES ------\n");
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    list_iterate(ts_tripulantes, &print_tstripulante);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
    log ? log_info(logger, "\n\n------------------------\n\n")
        :   printf("\n\n------------------------\n\n");
}

static void print_entrada_tp_t(void* x) {
    entrada_tp_t* elem = (entrada_tp_t*) x;
    ynlog
        ? log_info(logger,
            "PAG: %" PRIu32 " | FRAME: %" PRIu32 " | U (%" PRIu16 ") | P (%" PRIu16 ") | TUR (%" PRIu32 ")\n",
            elem->nro_pagina, elem->nro_frame, elem->bit_U, elem->bit_P, elem->TUR
        )
        : printf(
            "PAG: %" PRIu32 " | FRAME: %" PRIu32 " | U (%" PRIu16 ") | P (%" PRIu16 ") | TUR (%" PRIu32 ")\n",
            elem->nro_pagina, elem->nro_frame, elem->bit_U, elem->bit_P, elem->TUR
        );
    ;
}
static void print_tppatota(void* x) {
    tp_patota_t* elem = (tp_patota_t*) x;
    if (ynlog) {
        log_info(
            logger,
            "PID: %" PRIu32 " | TRIPS: %" PRIu32 " | INIT: %" PRIu32 " | T_TAREAS: %" PRIu32 " | PAGES: %" PRIu32 "\n",
            elem->pid, elem->tripulantes_totales, elem->tripulantes_inicializados, elem->tamanio_tareas, elem->pages
        );
        list_iterate(elem->paginas, &print_entrada_tp_t);
        //list_iterate(elem->posiciones, &print_t_posicion1);
    }
    else {
        printf(
            "PID: %" PRIu32 " | TRIPS: %" PRIu32 " | INIT: %" PRIu32 " | T_TAREAS: %" PRIu32 " | PAGES: %" PRIu32 "\n",
            elem->pid, elem->tripulantes_totales, elem->tripulantes_inicializados, elem->tamanio_tareas, elem->pages
        );
        list_iterate(elem->paginas, &print_entrada_tp_t);
        //list_iterate(elem->posiciones, &print_t_posicion1);
    }
}
void print_tppatotas(bool log) {
    ynlog = log;
    log ? log_info(logger, "\n\n------ TP PATOTAS ------")
        : printf("\n\n------ TP PATOTAS ------\n");
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    list_iterate(tp_patotas, &print_tppatota);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
    log ? log_info(logger, "\n\n------------------------\n\n")
        :   printf("\n\n------------------------\n\n");
}

static void print_tid_pid_lookup_t(void* x) {
    tid_pid_lookup_t* elem = (tid_pid_lookup_t*) x;
    ynlog
        ? log_info(logger,
                 "TID: %" PRIu32 " | PID: %" PRIu32 " | AT (P. %" PRIu32 ">>%" PRIu32 ")",
            elem->tid, elem->pid, elem->nro_pagina, elem->inicio)
        : printf("TID: %" PRIu32 " | PID: %" PRIu32 " | AT (P. %" PRIu32 ">>%" PRIu32 ")\n",
            elem->tid, elem->pid, elem->nro_pagina, elem->inicio);
}
void print_tid_pid_lookup(bool log) {
    ynlog = log;
    log ? log_info(logger, "\n\n------ TID PID LOOKUP ------")
        : printf("\n\n------ TID PID LOOKUP ------\n");
    pthread_mutex_lock(&MUTEX_TID_PID_LOOKUP);
    list_iterate(tid_pid_lookup, &print_tid_pid_lookup_t);
    pthread_mutex_unlock(&MUTEX_TID_PID_LOOKUP);
    log ? log_info(logger, "\n\n------------------------\n\n")
        :   printf("\n\n------------------------\n\n");
}

void print_swap(bool log) {
    log_info(logger, "\n\n------ SWAP ------");
    for (signed int i = 0; i<cfg->TAMANIO_SWAP/cfg->TAMANIO_PAGINA; i++) {
        log_info(logger,
            "PID: %" PRIu32 " | NRO_PAG: %" PRIu32 " | INICIO: %" PRIu32,
            tabla_frames_swap[i].pid, tabla_frames_swap[i].nro_pagina, tabla_frames_swap[i].inicio
        );
    }
}

////// prints (logs)

void log_structures(uint16_t options) {
    if (options & PRI_MP) {
        char* dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
        char* dumpswap = mem_hexstring(area_swap, cfg->TAMANIO_SWAP);
        log_info(logger, "%s", dumpcito);
        log_info(logger, "\n---Swap---\n%s", dumpswap);
        free(dumpcito);
        free(dumpswap);
    }
    if (options & PRI_SEGLIB)            LOGPRINT            (seglib);
    if (options & PRI_SEGUS)             LOGPRINT             (segus);
    if (options & PRI_TSPATOTAS)         LOGPRINT         (tspatotas);
    if (options & PRI_TSTRIPULANTES)     LOGPRINT     (tstripulantes);
    if (options & PRI_TPPATOTAS)         LOGPRINT         (tppatotas);
    if (options & PRI_FRAMO)             LOGPRINT             (framo);
    if (options & PRI_TID_PID_LOOKUP)    LOGPRINT    (tid_pid_lookup);
    if (options & PRI_SWAP)              LOGPRINT              (swap);
}
