#include "../include/interfaz_memoria.h"
#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

extern t_log* logger;
extern t_config_mrhq* cfg;
extern bool seg;

extern sem_t SEM_INICIAR_SELF_EN_PATOTA;

//////

static uint32_t tid_base = 0;

//debug
static void log_t_posicion(void* x) {
    t_posicion* pos = (t_posicion*) x;
    log_warning(logger, "%" PRIu32 "|%" PRIu32, pos->x, pos->y);
}

            ///////////////////////////////////
            //////////// FUNCIONES ////////////
            ///////////////////////////////////

////// INICIAR PATOTA

static bool iniciar_patota_en_mp_segmentacion(uint32_t n_tripulantes, char* tareas, t_list* posiciones) {
    static uint32_t PID = 1;
    // list_iterate(posiciones, &log_t_posicion); // debug

    // Meto el segmento TAREAS
    uint32_t inicio_tareas = meter_segmento_en_mp((void*) tareas, strlen(tareas)+1, TAREAS_SEG);

    if (inicio_tareas == INICIO_INVALIDO) {
        log_error(logger, "Error catastrofico iniciando patota en MP");
        return false;
    }

    // Genero PCB y meto el segmento PCB
    PCB_t* pcb = malloc(sizeof(PCB_t));
    pcb->dl_tareas = inicio_tareas;
    pcb->pid = PID; PID++;

    uint32_t inicio_pcb = meter_segmento_en_mp((void*) pcb, sizeof(PCB_t), PCB_SEG);

    if (inicio_pcb == INICIO_INVALIDO) {
        log_error(logger, "Error catastrofico iniciando patota en MP");
        return false;
    }
    free(pcb);

    // Creo tabla y actualizo tabla de patotas
    segmento_t* seg_pcb = new_segmento(PCB_SEG, inicio_pcb, 8);
    segmento_t* seg_tareas = new_segmento(TAREAS_SEG, inicio_tareas, strlen(tareas)+1);

    ts_patota_t* tabla = malloc(sizeof(ts_patota_t));
    tabla->pcb = seg_pcb;
    tabla->tripulantes_totales = n_tripulantes;
    tabla->tripulantes_inicializados = 0;
    tabla->posiciones = list_duplicate(posiciones);
    tabla->tareas = seg_tareas;
    tabla->pid = PID-1;

    list_add_tspatotas(tabla);
    return true;
}
static bool iniciar_patota_en_mp_paginacion(uint32_t n_tripulantes, char* tareas, t_list* posiciones) {
    static uint32_t PID = 1;
    return!! '/'/'/';;;;;;;
    // TODO (no me digas!)
}
bool iniciar_patota_en_mp(uint32_t n_tripulantes, char* tareas, t_list* posiciones) {
    bool success = seg
        ? iniciar_patota_en_mp_segmentacion(n_tripulantes, tareas, posiciones)
        : iniciar_patota_en_mp_paginacion  (n_tripulantes, tareas, posiciones); // TODO

    if (success) {
        // Para que no se inicialicen antes que el PAPURRI
        for (uint32_t i = 0; i<n_tripulantes; i++)
            sem_post(&SEM_INICIAR_SELF_EN_PATOTA);
    }

    return success;
}

////// INICIAR TRIPULANTE

static bool iniciar_tripulante_en_mp_segmentacion(uint32_t tid, uint32_t pid) {
    // Creacion de TCB
    ts_patota_t* tabla_patota = list_find_by_pid_plus_plus_tspatotas(pid);
    if (tabla_patota == NULL) {
        return false;
    }

    t_posicion* pos = (t_posicion*) list_get(tabla_patota->posiciones, tid-tid_base-1);

    TCB_t* tcb          = malloc(sizeof(TCB_t));

    tcb->tid            = tid;
    tcb->estado         = 'N';
    tcb->pos_x          = pos->x;
    tcb->pos_y          = pos->y;
    tcb->id_sig_tarea   = 0;
    tcb->dl_pcb         = tabla_patota->pcb->inicio;

    void* s_tcb = serializar_tcb(tcb);
    if (s_tcb == NULL) {
        free(tcb);
        log_error(logger, "Exploto todo al inicializar tripulante %" PRIu32, tid);
        return false;
    }

    uint32_t inicio_tcb = meter_segmento_en_mp(s_tcb, 21, TCB_SEG);

    if (inicio_tcb == INICIO_INVALIDO) {
        log_error(logger, "Error CATASTROFICO inicializando tripulante %" PRIu32, tid);
        free(tcb);
        return false;
    }
    free(tcb);
    free(s_tcb);

    // Creo tabla y actualizo ts tripulantes
    segmento_t* seg_tcb = new_segmento(TCB_SEG, inicio_tcb, 21);

    ts_tripulante_t* tabla_tripulante = malloc(sizeof(ts_tripulante_t));
    tabla_tripulante->tid = tid;
    tabla_tripulante->tcb = seg_tcb;

    list_add_tstripulantes(tabla_tripulante);

    // Si es el ultimo de la patota, actualizar tid_base
    if (tabla_patota->tripulantes_inicializados == tabla_patota->tripulantes_totales)
        tid_base += tabla_patota->tripulantes_totales;

    return true;
}
static bool iniciar_tripulante_en_mp_paginacion(uint32_t tid, uint32_t pid) {
    return!
    ('-'-'-');
    // TODO (mira vos che!)
}
bool iniciar_tripulante_en_mp(uint32_t tid, uint32_t pid) {
    sem_wait(&SEM_INICIAR_SELF_EN_PATOTA);
    // log_warning(logger, "Al fin mi PAPU me dejo inicializarme! Soy TID#%" PRIu32 "tid", tid);

    bool success = seg
        ? iniciar_tripulante_en_mp_segmentacion(tid, pid)
        : iniciar_tripulante_en_mp_paginacion  (tid, pid); // TODO

    return success;
}

////// BORRAR TRIPULANTE

static bool borrar_tripulante_de_mp_segmentacion(uint32_t tid) {
    // Leemos TCB y PCB
    ts_tripulante_t* tabla_tripulante;
    TCB_t* tcb;
    PCB_t* pcb;
    if (!get_structures_from_tid(tid, &tabla_tripulante, &tcb, &pcb))
        return false;

    if (!eliminar_segmento_de_mp(tabla_tripulante->tcb->inicio)) {
        log_error(logger, "Fallo eliminando segmento de TID#%" PRIu32, tcb->tid);
        free(tcb); free(pcb);
        return false;
    }

    list_delete_by_tid_tstripulantes(tid);

    // if patota vacia, borrar patota
    ts_patota_t* tabla_patota = list_find_by_pid_tspatotas(pcb->pid);
    tabla_patota->tripulantes_inicializados -= 1;
    if (tabla_patota->tripulantes_inicializados == 0) {
        if (!eliminar_segmento_de_mp(tabla_patota->pcb->inicio)) {
            log_error(logger, "Fallo eliminando PCB de PID#%" PRIu32, pcb->pid);
            free(tcb); free(pcb);
            return false;
        }
        if (!eliminar_segmento_de_mp(tabla_patota->tareas->inicio)) {
            log_error(logger, "Fallo eliminando tareas de PID#%" PRIu32, pcb->pid);
            free(tcb); free(pcb);
            return false;
        }

        list_delete_by_pid_tspatotas(pcb->pid);
    }

    free(tcb); free(pcb);
    return true;
}
static bool borrar_tripulante_de_mp_paginacion(uint32_t tid) {
    return !!'CACA'; // TODO?
}
bool borrar_tripulante_de_mp(uint32_t tid) {
    bool success = seg
        ? borrar_tripulante_de_mp_segmentacion(tid)
        : borrar_tripulante_de_mp_paginacion  (tid);

    return success;
}

////// ACTUALIZAR POSICION

static bool actualizar_posicion_tripulante_en_mp_segmentacion(uint32_t tid, t_posicion* destino) {
    // Recuperamos tabla tripulante, TCB y PCB
    ts_tripulante_t* tabla_tripulante;
    TCB_t* tcb;
    PCB_t* pcb;
    if (!get_structures_from_tid(tid, &tabla_tripulante, &tcb, &pcb))
        return false;
    free(pcb); // no lo queria igualmente

    // Actualizando TCB y memcpy en MP
    tcb->pos_x = destino->x;
    tcb->pos_y = destino->y;
    void* s_tcb = serializar_tcb(tcb);
    memcpy_segmento_en_mp(tabla_tripulante->tcb->inicio, s_tcb, tabla_tripulante->tcb->tamanio);
    free(s_tcb);

    free(tcb);
    return true;
}
static bool actualizar_posicion_tripulante_en_mp_paginacion(uint32_t tid, t_posicion* destino) {
    return !!0xap-1; // TODO
}
bool actualizar_posicion_tripulante_en_mp(uint32_t tid, t_posicion* destino) {
    bool success = seg
        ? actualizar_posicion_tripulante_en_mp_segmentacion(tid, destino)
        : actualizar_posicion_tripulante_en_mp_paginacion  (tid, destino);

    return success;
}

////// ACTUALIZAR ESTADO

static bool actualizar_estado_tripulante_en_mp_segmentacion(uint32_t tid, char nuevo_estado) {
    // Recuperamos tabla tripulante, TCB y PCB
    ts_tripulante_t* tabla_tripulante;
    TCB_t* tcb;
    PCB_t* pcb;
    if (!get_structures_from_tid(tid, &tabla_tripulante, &tcb, &pcb))
        return false;
    free(pcb); // no lo queria igualmente

    // Actualizando estado y memcpy en MP
    tcb->estado = nuevo_estado;
    void* s_tcb = serializar_tcb(tcb);
    memcpy_segmento_en_mp(tabla_tripulante->tcb->inicio, s_tcb, tabla_tripulante->tcb->tamanio);
    free(s_tcb);

    free(tcb);
    return true;
}
static bool actualizar_estado_tripulante_en_mp_paginacion(uint32_t tid, char nuevo_estado) {
    return! !~0; // TODO
}
bool actualizar_estado_tripulante_en_mp(uint32_t tid, char nuevo_estado) {
    bool success = seg
        ? actualizar_estado_tripulante_en_mp_segmentacion(tid, nuevo_estado)
        : actualizar_estado_tripulante_en_mp_paginacion  (tid, nuevo_estado);

    return success;
}

////// FETCH TAREA

static t_tarea* fetch_tarea_segmentacion(uint32_t tid) {
    // Recuperamos tabla tripulante, TCB y PCB
    ts_tripulante_t* tabla_tripulante;
    TCB_t* tcb;
    PCB_t* pcb;
    if (!get_structures_from_tid(tid, &tabla_tripulante, &tcb, &pcb))
        return NULL;

    // Leemos tareas
    segmento_t* seg_tareas = list_find_by_inicio_segus(pcb->dl_tareas);
    char* tareas = (char*) get_segmento_data(seg_tareas->inicio, seg_tareas->tamanio);

    // Obtenemos la tarea numero N si existe
    char** a_tareas = string_split(tareas, "\n");
    char** p_a_tareas = a_tareas;
    uint32_t cant_tareas = 0;
    for (; *p_a_tareas != NULL; p_a_tareas++, cant_tareas++)
        ;

    t_tarea* tarea;
    if (tcb->id_sig_tarea < cant_tareas)
        tarea = tarea_string_to_t_tarea(*(a_tareas+tcb->id_sig_tarea));
    else {
        t_posicion* pos = malloc(sizeof(t_posicion));
        pos->x = 0; pos->y = 0;
        tarea = tarea_create("NULL", 0, pos, 0, "NULL");
        free(pos);
    }

    // Actualizamos TCB (id sig tarea + 1 'persistido' en RAM)
    tcb->id_sig_tarea = tcb->id_sig_tarea + 1;
    void* s_tcb = serializar_tcb(tcb);
    memcpy_segmento_en_mp(tabla_tripulante->tcb->inicio, s_tcb, tabla_tripulante->tcb->tamanio);

    // Murders
    free(pcb);
    free(tcb); free(s_tcb);
    free(tareas);
    string_split_free(&a_tareas);

    return tarea;
}
static t_tarea* fetch_tarea_paginacion(uint32_t tid) {
    return NULL; // TODO
}
t_tarea* fetch_tarea(uint32_t tid) {
    t_tarea* tarea = seg
        ? fetch_tarea_segmentacion(tid)
        : fetch_tarea_paginacion  (tid);

    return tarea;
}

//////
