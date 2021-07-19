#include "../include/interfaz_memoria.h"
#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

extern t_log* logger;
extern t_config_mrhq* cfg;

extern sem_t SEM_INICIAR_SELF_EN_PATOTA;

extern pthread_mutex_t MUTEX_MP_BUSY;

extern void* memoria_principal;
extern void* area_swap;

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

static uint32_t PID = 1;
static bool iniciar_patota_en_mp_segmentacion(uint32_t n_tripulantes, char* tareas, t_list* posiciones) {
    //static uint32_t PID = 1;
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
    segmento_t* seg_tareas = new_segmento(TAREAS_SEG, 0, inicio_tareas, strlen(tareas)+1);
    segmento_t* seg_pcb = new_segmento(PCB_SEG, 1, inicio_pcb, 8);

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
    //static uint32_t PID = 1;

    // Primero TAREAS, despues PCB
    void* data = malloc(strlen(tareas) + 1 + 8);

    PCB_t* pcb = malloc(sizeof(PCB_t));
    pcb->pid = PID;
    pcb->dl_tareas = 0;
    
    memcpy(data, tareas, strlen(tareas)+1);
    memcpy(data+strlen(tareas)+1, pcb, 8);
    free(pcb);

    // Tabla administrativa
    tp_patota_t* tabla = malloc(sizeof(tp_patota_t));
    tabla->pid = PID;
    tabla->tripulantes_totales = n_tripulantes;
    tabla->tripulantes_inicializados = 0;
    tabla->tamanio_tareas = strlen(tareas)+1;
    tabla->pages = 0;
    tabla->posiciones = list_duplicate(posiciones);
    tabla->paginas = list_create();

    list_add_tppatotas(tabla);

    bool nuevapag; // ignorable, ver uso en iniciar_tripulante
    //log_info(logger, "Por appendear data de patota %" PRIu32 " en memoria...", PID);
    uint32_t inicio = append_data_to_patota_en_mp(data, strlen(tareas)+1+8, PID++, &nuevapag);
    free(data);

    return inicio!=0xFFFF;
}
bool iniciar_patota_en_mp(uint32_t n_tripulantes, char* tareas, t_list* posiciones) {
    log_info(logger, "Iniciando patota %" PRIu32 " en memoria (TAREAS y PCB)", PID);
    bool success = cfg->SEG
        ? iniciar_patota_en_mp_segmentacion(n_tripulantes, tareas, posiciones)
        : iniciar_patota_en_mp_paginacion  (n_tripulantes, tareas, posiciones);

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
        free(s_tcb);
        return false;
    }
    free(tcb);
    free(s_tcb);

    // Creo tabla y actualizo ts tripulantes
    // El nro. de segmento es siempre 1 mas que trip. inicializados
    // porque segmento 0 y 1 son TAREAS y PCB (think about it)
    segmento_t* seg_tcb = new_segmento(
        TCB_SEG, tabla_patota->tripulantes_inicializados+1, inicio_tcb, 21
    );

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
    // Posicion para ese tripulante
    //log_info(logger, "Inicializando tripulante TID#%" PRIu32, tid);
    tp_patota_t* tabla_patota = list_find_by_pid_plus_plus_tppatotas(pid);
    if (tabla_patota == NULL) {
        return false;
    }

    t_posicion* pos = (t_posicion*) list_get(tabla_patota->posiciones, tid-tid_base-1);
    uint32_t cant_paginas_antes = tabla_patota->pages;

    // Where the PCB at? My man? (Al final de las tareas, bud)
    uint32_t size_tareas = tabla_patota->tamanio_tareas;
    uint32_t pag_pcb = size_tareas / cfg->TAMANIO_PAGINA;
    uint32_t offset_pcb = size_tareas % cfg->TAMANIO_PAGINA;

    // Genero TCB
    TCB_t* tcb          = malloc(sizeof(TCB_t));

    tcb->tid            = tid;
    tcb->estado         = 'N';
    tcb->pos_x          = pos->x;
    tcb->pos_y          = pos->y;
    tcb->id_sig_tarea   = 0;
    tcb->dl_pcb         = (pag_pcb << 16) + offset_pcb;

    /*log_info(logger, "TID#%" PRIu32 " inicializado con pag_pcb: %" PRIu32 " , offset_pcb: %" PRIu32 " , tareas de tamanio %" PRIu32,
            tid, pag_pcb, offset_pcb, tabla_patota->tamanio_tareas
    );*/

    //log_info(logger, "DL_PCB at 0x%08" PRIx32 " for PID#%" PRIu32, tcb->dl_pcb, pid);

    void* s_tcb = serializar_tcb(tcb);
    if (s_tcb == NULL) {
        free(tcb);
        log_error(logger, "Exploto todo al inicializar tripulante %" PRIu32, tid);
        return false;
    }

    // Meto el TCB al final de lo que ya tengo cargado (sease, TAREAS + PCB)
    bool nuevapag;
    //log_info(logger, "Por appendear TCB de %" PRIu32 " en memoria...", tid);
    uint32_t inicio = append_data_to_patota_en_mp(s_tcb, 21, pid, &nuevapag);

    if (inicio == 0xFFFF) {
        log_error(logger, "Error CATASTROFICO inicializando tripulante %" PRIu32, tid);
        1+1;
    }
    else {
        // Si es el ultimo de la patota, actualizar tid_base
        if (tabla_patota->tripulantes_inicializados == tabla_patota->tripulantes_totales)
            tid_base += tabla_patota->tripulantes_totales;
    }

    //log_info(logger, "cant_paginas_antes: %" PRIu32 ", nuevapag: %d", cant_paginas_antes, nuevapag);

    // Agregar entrada lookup de PID
    tid_pid_lookup_t* lookup = malloc(sizeof(tid_pid_lookup_t));
    lookup->tid = tid;
    lookup->pid = pid;
    lookup->nro_pagina = cant_paginas_antes-1 + nuevapag;
    lookup->inicio = inicio;
    list_add_tid_pid_lookup(lookup);

    /*char* dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
    char* dumpswap = mem_hexstring(area_swap, cfg->TAMANIO_SWAP);
    log_info(logger, "%s", dumpcito);
    log_info(logger, "\n---Swap---\n%s", dumpswap);
    free(dumpcito);
    free(dumpswap);*/

    free(s_tcb);
    free(tcb);

    return inicio!=0xFFFF;
}
bool iniciar_tripulante_en_mp(uint32_t tid, uint32_t pid) {
    sem_wait(&SEM_INICIAR_SELF_EN_PATOTA);
    log_info(logger, "Iniciando tripulante TID#%" PRIu32 " en memoria (TCB)", tid);

    bool success = cfg->SEG
        ? iniciar_tripulante_en_mp_segmentacion(tid, pid)
        : iniciar_tripulante_en_mp_paginacion  (tid, pid);

    return success;
}

////// BORRAR TRIPULANTE

static bool borrar_tripulante_de_mp_segmentacion(uint32_t tid) {
    // Leemos TCB y PCB
    ts_tripulante_t* tabla_tripulante;
    TCB_t* tcb;
    PCB_t* pcb;
    if (!get_structures_from_tid_segmentacion(tid, &tabla_tripulante, &tcb, &pcb))
        return false;

    if (!eliminar_segmento_de_mp(tabla_tripulante->tcb->inicio)) {
        log_error(logger, "Fallo eliminando segmento de TID#%" PRIu32, tcb->tid);
        free(tcb); free(pcb);
        return false;
    }

    list_delete_by_tid_tstripulantes(tid);
    list_update_nro_seg_tcb_by_pid_tstripulantes(tid, pcb->pid);

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
    // Conseguir el pid de alguna forma partiendo del TID y llamar a
    auto uint32_t pid = pid_of_tid(tid);
    if (pid == 0xFFFF) return false;

    tp_patota_t* tabla_patota = list_find_by_pid_tppatotas(pid);
    tabla_patota->tripulantes_inicializados--;

    list_tid_pid_lookup_remove_by_tid(tid);

    bool ret;
    if (tabla_patota->tripulantes_inicializados == 0) {
        // ULTIMO TRIPULANTE
        //log_info(logger, "Borrando patota de TID#%" PRIu32 " (PID#%" PRIu32 ")", tid, pid);
        ret = delete_patota_en_mp(pid); // la borra de MP y estructuras admin.

        if (!ret) log_error(logger, "Error TERRIBLE borrando patota de MP");
    }
    else {
        // TRIPULANTE CUALQUIERA (no borramos TCB en PAGINACION,
        // borramos TODO cuando se expulsaron TODOS)
        return true;
    }

    return ret;
}
bool borrar_tripulante_de_mp(uint32_t tid) {
    log_info(logger, "Borrando tripulante TID#%" PRIu32, tid);
    bool success = cfg->SEG
        ? borrar_tripulante_de_mp_segmentacion(tid)
        : borrar_tripulante_de_mp_paginacion  (tid);

    return success;
}

////// ACTUALIZAR POSICION

static bool actualizar_posicion_tripulante_en_mp_segmentacion(uint32_t tid, t_posicion* destino) {
    // Recuperamos tabla tripulante, TCB y PCB
    //log_info(logger, "TID#%" PRIu32 " se mueve a %" PRIu32 ":%" PRIu32, tid, destino->x, destino->y);
    ts_tripulante_t* tabla_tripulante;
    TCB_t* tcb;
    PCB_t* pcb;
    if (!get_structures_from_tid_segmentacion(tid, &tabla_tripulante, &tcb, &pcb)) {
        log_error(logger, "Fallo epico recuperando estructuras del TID#%" PRIu32, tid);
        return false;
    }
    free(pcb); // no lo queria igualmente

    // Actualizando TCB y memcpy en MP
    tcb->pos_x = destino->x;
    tcb->pos_y = destino->y;
    void* s_tcb = serializar_tcb(tcb);
    //log_info(logger, "Antes de memcpy_segmento_en_mp...");
    memcpy_segmento_en_mp(tabla_tripulante->tcb->inicio, s_tcb, tabla_tripulante->tcb->tamanio);
    //log_info(logger, "Despues de memcpy_segmento_en_mp...");
    free(s_tcb);

    free(tcb);
    return true;
}
static bool actualizar_posicion_tripulante_en_mp_paginacion(uint32_t tid, t_posicion* destino) {
    // Recuperamos tabla, TCB, PCB
    //log_info(logger, "Actualizando posicion de TID#%" PRIu32 "->(%" PRIu32 ",%" PRIu32 ")",tid,destino->x,destino->y);
    tid_pid_lookup_t* tabla;
    TCB_t* tcb;
    PCB_t* pcb;

    bool ret = RACE_get_structures_from_tid_paginacion(tid, &tabla, &tcb, &pcb);
    if (!ret) return false;

    tcb->pos_x = destino->x;
    tcb->pos_y = destino->y;
    
    bool success = RACE_actualizar_tcb_en_mp(pcb->pid, tcb);

    free(tcb);
    free(pcb);

    return success;
}
bool actualizar_posicion_tripulante_en_mp(uint32_t tid, t_posicion* destino) {
    log_info(logger, "Actualizando posicion de TID#%" PRIu32 " a %" PRIu32 "|%" PRIu32,
        tid, destino->x, destino->y
    );
    bool success = cfg->SEG
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
    if (!get_structures_from_tid_segmentacion(tid, &tabla_tripulante, &tcb, &pcb))
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
    // Recuperamos tabla, TCB, PCB
    //log_info(logger, "Actualizando estado de TID#%" PRIu32 "->%c", tid, nuevo_estado);
    tid_pid_lookup_t* tabla;
    TCB_t* tcb;
    PCB_t* pcb;

    /*char* dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
    char* dumpswap = mem_hexstring(area_swap, cfg->TAMANIO_SWAP);
    log_info(logger, "%s", dumpcito);
    log_info(logger, "\n---Swap---\n%s", dumpswap);
    free(dumpcito);
    free(dumpswap);*/

    bool ret = RACE_get_structures_from_tid_paginacion(tid, &tabla, &tcb, &pcb);
    if (!ret) return false;

    tcb->estado = nuevo_estado;

    bool success = RACE_actualizar_tcb_en_mp(pcb->pid, tcb);

    /*dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
    dumpswap = mem_hexstring(area_swap, cfg->TAMANIO_SWAP);
    log_info(logger, "%s", dumpcito);
    log_info(logger, "\n---Swap---\n%s", dumpswap);
    free(dumpcito);
    free(dumpswap);*/

    free(tcb);
    free(pcb);

    return success;
}
bool actualizar_estado_tripulante_en_mp(uint32_t tid, char nuevo_estado) {
    log_info(logger, "Actualizando estado de TID#%" PRIu32 " a %c", tid, nuevo_estado);
    bool success = cfg->SEG
        ? actualizar_estado_tripulante_en_mp_segmentacion(tid, nuevo_estado)
        : actualizar_estado_tripulante_en_mp_paginacion  (tid, nuevo_estado);

    return success;
}

////// FETCH TAREA

static t_tarea* tarea_numero_n_si_existe(char* tareas, uint32_t tarea_n) {
    char** a_tareas = string_split(tareas, "\n");

    // Cuento cuantas hay
    char** p_a_tareas = a_tareas;
    uint32_t cant_tareas = 0;
    for (; *p_a_tareas != NULL; p_a_tareas++, cant_tareas++)
        ;

    // Enesima tarea o tarea null
    t_tarea* tarea;
    if (tarea_n < cant_tareas)
        tarea = tarea_string_to_t_tarea(*(a_tareas+tarea_n));
    else {
        t_posicion* pos = malloc(sizeof(t_posicion));
        pos->x = 0; pos->y = 0;
        tarea = tarea_create("NULL", 0, pos, 0, "NULL");
        free(pos);
    }

    string_split_free(&a_tareas);

    return tarea;
}

static t_tarea* fetch_tarea_segmentacion(uint32_t tid) {
    // Recuperamos tabla tripulante, TCB y PCB
    ts_tripulante_t* tabla_tripulante = NULL;
    TCB_t* tcb = NULL;
    PCB_t* pcb = NULL;
    if (!get_structures_from_tid_segmentacion(tid, &tabla_tripulante, &tcb, &pcb)) {
        // exploto todo
        log_error(logger, "Error MUY FEO recuperando tarea para TID#%" PRIu32, tid);
        free(tcb); free(pcb);
        return NULL;
    }

    // Leemos tareas
    segmento_t* seg_tareas = list_find_by_inicio_segus(pcb->dl_tareas);
    char* tareas = (char*) get_segmento_data(seg_tareas->inicio, seg_tareas->tamanio);

    // Obtenemos la tarea numero N si existe
    t_tarea* tarea = tarea_numero_n_si_existe(tareas, tcb->id_sig_tarea);
    free(tareas);

    // Actualizamos TCB (id sig tarea + 1 'persistido' en RAM)
    tcb->id_sig_tarea = tcb->id_sig_tarea + 1;
    void* s_tcb = serializar_tcb(tcb);
    memcpy_segmento_en_mp(tabla_tripulante->tcb->inicio, s_tcb, tabla_tripulante->tcb->tamanio);

    // Murders
    free(pcb);
    free(tcb); free(s_tcb);

    return tarea;
}
static t_tarea* fetch_tarea_paginacion(uint32_t tid) {
    // Estructuras relacionadas al TID
    //log_info(logger, "Buscando tarea para TID#%" PRIu32 "...", tid);

    /*char* dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
    char* dumpswap = mem_hexstring(area_swap, cfg->TAMANIO_SWAP);
    log_info(logger, "%s", dumpcito);
    log_info(logger, "\n---Swap---\n%s", dumpswap);
    free(dumpcito);
    free(dumpswap);*/

    //print_tid_pid_lookup(true);

    tid_pid_lookup_t* tabla = NULL;
    TCB_t* tcb = NULL;
    PCB_t* pcb = NULL;

    if (!RACE_get_structures_from_tid_paginacion(tid, &tabla, &tcb, &pcb)) {
        // exploto todo
        log_error(logger, "Error MUY FEO recuperando tarea para TID#%" PRIu32, tid);
        free(tcb); free(pcb);
        return NULL;
    }

    // LEO TAREAS DE MP
    tp_patota_t* tabla_patota = list_find_by_pid_tppatotas(pcb->pid);

    char* tareas = (char*) RACE_read_from_mp_pid_pagina_offset_tamanio(
        pcb->pid, pcb->dl_tareas>>16, pcb->dl_tareas&0x00FF, tabla_patota->tamanio_tareas
    );

    // Obtenemos la tarea numero N si existe
    t_tarea* tarea = tarea_numero_n_si_existe(tareas, tcb->id_sig_tarea);
    free(tareas);

    // Actualizamos TCB (id sig tarea + 1 'persistido' en RAM)
    tcb->id_sig_tarea = tcb->id_sig_tarea + 1;
    if (!RACE_actualizar_tcb_en_mp(pcb->pid, tcb)) {
        log_error(logger, "Fallo actualizando TCB en memoria para TID#%" PRIu32, tid);
    }

    free(tcb);
    free(pcb);

    /*dumpcito = mem_hexstring(memoria_principal, cfg->TAMANIO_MEMORIA);
    dumpswap = mem_hexstring(area_swap, cfg->TAMANIO_SWAP);
    log_info(logger, "%s", dumpcito);
    log_info(logger, "\n---Swap---\n%s", dumpswap);
    free(dumpcito);
    free(dumpswap);*/

    return tarea;
}
t_tarea* fetch_tarea(uint32_t tid) {
    log_info(logger, "Buscando proxima tarea para TID#%" PRIu32, tid);
    t_tarea* tarea = cfg->SEG
        ? fetch_tarea_segmentacion(tid)
        : fetch_tarea_paginacion  (tid);

    return tarea;
}

//////
