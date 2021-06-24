#include "../include/interfaz_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

bool iniciar_patota_en_mp(char* tareas, t_list* posiciones) {
    // TODO: Contemplar paginacion
    bool segmentacion = strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION") == 0;

    static uint32_t PID = 1;

    if (segmentacion) {

        // Meto el segmento TAREAS
        uint32_t inicio_tareas = meter_segmento_en_mp((void*) tareas, strlen(tareas)+1);
        if (inicio_tareas == INICIO_INVALIDO) {
            log_error(logger, "Error catastrofico iniciando patota en MP");
            return false;
        }

        // Genero PCB y meto el segmento PCB
        PCB_t* pcb = malloc(sizeof(PCB_t));
        pcb->dl_tareas = 1;
        pcb->pid = PID; PID++;

        // no tengo idea por que pero si no hacia esto tiraba invalid read
        void* s_pcb = malloc(sizeof(PCB_t));
        memcpy(s_pcb, pcb, sizeof(PCB_t));

        uint32_t inicio_pcb = meter_segmento_en_mp(s_pcb, sizeof(PCB_t));
        if (inicio_pcb == INICIO_INVALIDO) {
            log_error(logger, "Error catastrofico iniciando patota en MP");
            return false;
        }
        free(pcb); free(s_pcb);

        // Creo tabla y actualizo tabla de patotas
        segmento_t* seg_pcb = new_segmento(0, inicio_pcb, 8);
        segmento_t* seg_tareas = new_segmento(1, inicio_tareas, strlen(tareas)+1);

        ts_patota_t* tabla = malloc(sizeof(ts_patota_t));
        tabla->pcb = seg_pcb;
        tabla->tripulantes_inicializados = 0;
        tabla->posiciones = list_duplicate(posiciones);
        tabla->tareas = seg_tareas;
        tabla->pid = PID-1;

        list_add_tspatotas(tabla);
    }

    else {
        // TODO: Contemplar paginacion (posiblemente cambiar para que no haya un if-else enorme)
    }

    return true;
}

bool iniciar_tripulante_en_mp(uint32_t tid, uint32_t pid) {
    // TODO: Contemplar paginacion
    bool segmentacion = strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION") == 0;

    // Creacion de TCB
    ts_patota_t* tabla = list_find_by_pid_plus_plus_tspatotas(pid);
    if (tabla == NULL) return false;

    t_posicion* pos = (t_posicion*) list_get(tabla->posiciones, tabla->tripulantes_inicializados-1);

    TCB_t* tcb           = malloc(sizeof(TCB_t));

    tcb->tid            = tid;
    tcb->estado         = 'N';
    tcb->pos_x          = pos->x;
    tcb->pos_y          = pos->y;
    tcb->id_sig_tarea   = 0;
    tcb->dl_pcb         = tabla->pcb->inicio;

    void* s_tcb = serializar_tcb(tcb);
    if (s_tcb == NULL) {
        free(tcb);
        log_error(logger, "Exploto todo al inicializar tripulante %" PRIu32, tid);
        return false;
    }

    // meter en MP
    if (segmentacion) {
        uint32_t inicio_tcb = meter_segmento_en_mp(s_tcb, sizeof(TCB_t));
        if (inicio_tcb == INICIO_INVALIDO) {
            log_error(logger, "Error CATASTROFICO inicializando tripulante %" PRIu32, tid);
            free(tcb);
            return false;
        }
        free(tcb);

        // Creo tabla y actualizo ts tripulantes
        segmento_t* seg_tcb = new_segmento(0, inicio_tcb, 21);

        ts_tripulante_t* tabla = malloc(sizeof(ts_tripulante_t));
        tabla->tid = tid;
        tabla->tcb = seg_tcb;

        list_add_tstripulantes(tabla);
    }
    else {
        // TODO: Contemplar paginacion (posiblemente cambiar para que no haya un if-else enorme)
    }

    return true;
}