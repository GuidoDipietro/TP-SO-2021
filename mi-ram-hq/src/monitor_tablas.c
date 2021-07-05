#include "../include/monitor_tablas.h"

extern t_log* logger;
extern t_config_mrhq* cfg;

extern pthread_mutex_t MUTEX_TS_PATOTAS;
extern pthread_mutex_t MUTEX_TS_TRIPULANTES;
extern pthread_mutex_t MUTEX_TP_PATOTAS;

extern t_list* ts_patotas;
extern t_list* ts_tripulantes;

extern t_list* tp_patotas;

static uint32_t static_pid;
static uint32_t static_tid;
static uint32_t static_inicio;

/// TS PATOTAS

static bool ts_patota_t_has_pid(void* x) {
    ts_patota_t* elem = (ts_patota_t*) x;
    return elem->pid == static_pid;
}

static bool ts_patota_t_pcb_has_inicio(void* x) {
    ts_patota_t* elem = (ts_patota_t*) x;
    return elem->pcb->inicio == static_inicio;
}

static bool ts_patota_t_tareas_has_inicio(void* x) {
    ts_patota_t* elem = (ts_patota_t*) x;
    return elem->tareas->inicio == static_inicio;
}

void list_add_tspatotas(ts_patota_t* elem) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_add(ts_patotas, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

void list_delete_by_pid_tspatotas(uint32_t pid) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_remove_and_destroy_by_condition(ts_patotas, &ts_patota_t_has_pid, &free_ts_patota_t);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

// Recupera entrada de la tabla y suma 1 a tripulantes inicializados
ts_patota_t* list_find_by_pid_plus_plus_tspatotas(uint32_t pid) {
    static_pid = pid;
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_t_has_pid);
    if (elem)
        elem->tripulantes_inicializados = elem->tripulantes_inicializados + 1;
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

ts_patota_t* list_find_by_pid_tspatotas(uint32_t pid) {
    static_pid = pid;
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_t_has_pid);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

ts_patota_t* list_find_by_inicio_pcb_tspatotas(uint32_t inicio) {
    static_inicio = inicio;
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    ts_patota_t* elem = list_find(ts_patotas, &ts_patota_t_pcb_has_inicio);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
    return elem;
}

ts_patota_t* list_find_by_inicio_tareas_tspatotas(uint32_t inicio) {
    static_inicio = inicio;
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
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
    static_tid = tid;
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    ts_tripulante_t* elem = list_find(ts_tripulantes, &ts_tripulante_t_has_tid);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
    return elem;
}

ts_tripulante_t* list_find_by_inicio_tcb_tstripulantes(uint32_t inicio) {
    static_inicio = inicio;
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
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

void asesinar_tppatotas() {
    pthread_mutex_lock(&MUTEX_TP_PATOTAS);
    list_destroy_and_destroy_elements(tp_patotas, &free_tp_patota_t);
    pthread_mutex_unlock(&MUTEX_TP_PATOTAS);
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

////// prints (logs)

void log_structures(uint8_t options) {
    if (options & PRI_MP) {
        char* dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
        log_info(logger, "%s", dumpcito);
        free(dumpcito);
    }
    if (options & PRI_SEGLIB)         LOGPRINT         (seglib);
    if (options & PRI_SEGUS)          LOGPRINT          (segus);
    if (options & PRI_TSPATOTAS)      LOGPRINT      (tspatotas);
    if (options & PRI_TSTRIPULANTES)  LOGPRINT  (tstripulantes);
    if (options & PRI_FRAMO)          LOGPRINT          (framo);
}
