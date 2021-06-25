#include "../include/interfaz_memoria.h"
#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

extern t_log* logger;
extern t_config_mrhq* cfg;

extern sem_t SEM_INICIAR_SELF_EN_PATOTA;

////// Funcs

static uint32_t tid_base = 0;

//debug
static void log_t_posicion(void* x) {
    t_posicion* pos = (t_posicion*) x;
    log_warning(logger, "%" PRIu32 "|%" PRIu32, pos->x, pos->y);
}

bool iniciar_patota_en_mp(uint32_t n_tripulantes, char* tareas, t_list* posiciones) {
    // TODO: Contemplar paginacion
    bool segmentacion = strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION") == 0;

    static uint32_t PID = 1;

    if (segmentacion) {

        list_iterate(posiciones, &log_t_posicion); // debug

        // Meto el segmento TAREAS
        uint32_t inicio_tareas = meter_segmento_en_mp((void*) tareas, strlen(tareas)+1);
        if (inicio_tareas == INICIO_INVALIDO) {
            log_error(logger, "Error catastrofico iniciando patota en MP");
            return false;
        }

        // Genero PCB y meto el segmento PCB
        PCB_t* pcb = malloc(sizeof(PCB_t));
        pcb->dl_tareas = inicio_tareas;
        pcb->pid = PID; PID++;

        uint32_t inicio_pcb = meter_segmento_en_mp((void*) pcb, sizeof(PCB_t));
        if (inicio_pcb == INICIO_INVALIDO) {
            log_error(logger, "Error catastrofico iniciando patota en MP");
            return false;
        }
        free(pcb);

        // Creo tabla y actualizo tabla de patotas
        segmento_t* seg_pcb = new_segmento(0, inicio_pcb, 8);
        segmento_t* seg_tareas = new_segmento(1, inicio_tareas, strlen(tareas)+1);

        ts_patota_t* tabla = malloc(sizeof(ts_patota_t));
        tabla->pcb = seg_pcb;
        tabla->tripulantes_totales = n_tripulantes;
        tabla->tripulantes_inicializados = 0;
        tabla->posiciones = list_duplicate(posiciones);
        tabla->tareas = seg_tareas;
        tabla->pid = PID-1;

        list_add_tspatotas(tabla);

        // Para que no se inicialicen antes que el PAPURRI
        for (uint32_t i = 0; i<n_tripulantes; i++)
            sem_post(&SEM_INICIAR_SELF_EN_PATOTA);
    }

    else {
        // TODO: Contemplar paginacion (posiblemente cambiar para que no haya un if-else enorme)
    }

    return true;
}

bool iniciar_tripulante_en_mp(uint32_t tid, uint32_t pid) {
    sem_wait(&SEM_INICIAR_SELF_EN_PATOTA);

    // TODO: Contemplar paginacion
    bool segmentacion = strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION") == 0;

    // Creacion de TCB
    ts_patota_t* tabla_patota = list_find_by_pid_plus_plus_tspatotas(pid);
    if (tabla_patota == NULL) return false;

    t_posicion* pos = (t_posicion*) list_get(tabla_patota->posiciones, tid-tid_base-1);

    TCB_t* tcb           = malloc(sizeof(TCB_t));

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

    // meter en MP
    if (segmentacion) {
        uint32_t inicio_tcb = meter_segmento_en_mp(s_tcb, 21);
        if (inicio_tcb == INICIO_INVALIDO) {
            log_error(logger, "Error CATASTROFICO inicializando tripulante %" PRIu32, tid);
            free(tcb);
            return false;
        }
        free(tcb);
        free(s_tcb);

        // Creo tabla y actualizo ts tripulantes
        segmento_t* seg_tcb = new_segmento(0, inicio_tcb, 21);

        ts_tripulante_t* tabla_tripulante = malloc(sizeof(ts_tripulante_t));
        tabla_tripulante->tid = tid;
        tabla_tripulante->tcb = seg_tcb;

        list_add_tstripulantes(tabla_tripulante);

        // Si es el ultimo de la patota, actualizar tid_base
        if (tabla_patota->tripulantes_inicializados == tabla_patota->tripulantes_totales)
            tid_base += tabla_patota->tripulantes_totales;
    }
    else {
        // TODO: Contemplar paginacion (posiblemente cambiar para que no haya un if-else enorme)
    }

    return true;
}

t_tarea* fetch_tarea(uint32_t tid) {
    ts_tripulante_t* tabla_tripulante = list_find_by_tid_tstripulantes(tid);
    if (tabla_tripulante == NULL) return NULL;

    void* s_tcb = get_segmento_data(
        tabla_tripulante->tcb->inicio,
        tabla_tripulante->tcb->tamanio
    );
    TCB_t* tcb = deserializar_tcb(s_tcb);

    log_info(
        logger, "TID #%" PRIu32 " pide t[%" PRIu32 "], PCB en %" PRIu32 "",
        tid, tcb->id_sig_tarea, tcb->dl_pcb
    );

    void* s_pcb = get_segmento_data(tcb->dl_pcb, 8);
    PCB_t* pcb = deserializar_pcb(s_pcb);
    log_info(
        logger, "PID #%" PRIu32 ", tareas en %" PRIu32,
        pcb->pid, pcb->dl_tareas
    );

    t_posicion* pos = malloc(sizeof(t_posicion));
    pos->x = 0; pos->y = 0;
    t_tarea* tarea_prueba = tarea_create("Ejemplito",3,pos,5,"TAOYU");
    free(pos);

    free(pcb); free(s_pcb);
    free(tcb); free(s_tcb);

    return tarea_prueba;
}