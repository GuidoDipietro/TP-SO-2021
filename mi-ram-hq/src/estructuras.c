#include "../include/estructuras.h"

/// Creacion

tp_patota_t* tp_patota_t_create() {
    tp_patota_t* struct_tabla = malloc(sizeof(tp_patota_t));
    struct_tabla->tabla = list_create();
    return struct_tabla;
}

/// Frees

void free_ts_patota_t(void* x) {
    if (x == NULL) return;
    ts_patota_t* elem = (ts_patota_t*) x;
    free(elem->pcb);
    free(elem->tareas);
    free(elem);
}

void free_ts_tripulante_t(void* x) {
    if (x == NULL) return;
    ts_tripulante_t* elem = (ts_tripulante_t*) x;
    free(elem->tcb);
    free(elem);
}

void free_tp_patota_t(void* x) {
    if (x == NULL) return;
    tp_patota_t* elem = (tp_patota_t*) x;
    list_destroy_and_destroy_elements(elem->tabla, (void*) free);
    free(elem);
}

/// Stringify

char* stringify_segmento_t(segmento_t* segmento) {
    if (segmento == NULL) return NULL;

    size_t size = 49; // 32 chars + %3 + %08 + %5 + \0 = 49
    char* string = malloc(size);
    snprintf(
        string, size,
        "SEGMENTO: %3" PRIu32 " | INICIO: 0x%08" PRIx32 " | TAM: %5" PRIu32 "b",
        segmento->nro_segmento, segmento->inicio, segmento->tamanio
    );
    return string;
}

char* stringify_ts_patota_t(ts_patota_t* tabla, char* timestamp) {
    char* rayitas = "------------------------------"; // 30 guiones
    char* string_pcb = stringify_segmento_t(tabla->pcb);
    char* string_tareas = stringify_segmento_t(tabla->tareas);

    // para algo tiene un hardware sumador la compu no?
    size_t size = 30+1+6+strlen(timestamp)+1+12+strlen(string_pcb)+1+12+strlen(string_tareas)+1+30+1+1;
    char* out = malloc(size);
    snprintf(out, size,
        "%s\n"                          /*  rayitas     (30+1)              */
        "Dump: %s\n"                    /*  timestamp   (6+timestamp+1)     */
        "PROCESO: %3" PRIu32 " | %s\n"  /*  pcb         (12+s_pcb+1)        */
        "PROCESO: %3" PRIu32 " | %s\n"  /*  tareas      (12+s_tareas+1)     */
        "%s\n",                         /*  rayitas     (30+1+1)            */
        rayitas, timestamp, tabla->pid, string_pcb, tabla->pid, string_tareas, rayitas
    );

    free(string_pcb); free(string_tareas);
    return out;
}

char* stringify_ts_tripulante_t(ts_tripulante_t* tabla, char* timestamp) {
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;
}

/// Serializacion! De nuevo

void* serializar_pcb(PCB_t* pcb) {
    void* choclo = malloc(8);
    if (choclo == NULL) return NULL;
    
    memcpy(choclo, &pcb->pid, sizeof(uint32_t));
    memcpy(choclo+sizeof(uint32_t), &pcb->dl_tareas, sizeof(uint32_t));

    return choclo;
}

void* serializar_tcb(TCB_t* tcb) {
    void* choclo = malloc(21);
    if (choclo == NULL) return NULL;

    memcpy(choclo, &tcb->tid, sizeof(uint32_t));
    memcpy(choclo+sizeof(uint32_t), &tcb->estado, sizeof(char));
    memcpy(choclo+sizeof(uint32_t)+sizeof(char), &tcb->pos_x, sizeof(uint32_t));
    memcpy(choclo+sizeof(uint32_t)+sizeof(char)+sizeof(uint32_t), &tcb->pos_y, sizeof(uint32_t));
    memcpy(choclo+sizeof(uint32_t)+sizeof(char)+sizeof(uint32_t)+sizeof(uint32_t), &tcb->id_sig_tarea, sizeof(uint32_t));
    memcpy(choclo+sizeof(uint32_t)+sizeof(char)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t), &tcb->dl_pcb, sizeof(uint32_t));

    return choclo;
}

void* serializar_string_tareas(char* string) {
    void* choclo = malloc(strlen(string)+1);
    memcpy(choclo, string, strlen(string)+1);
    return choclo; // equivalente a hacer (void*) string, pero bueno
}
