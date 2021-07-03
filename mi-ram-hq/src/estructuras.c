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
    list_destroy_and_destroy_elements(elem->posiciones, (void*) free);
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

/// Serializacion! De nuevo

void* serializar_pcb(PCB_t* pcb) {
    void* choclo = malloc(8);
    if (choclo == NULL) return NULL;
    
    memcpy(choclo, &pcb->pid, sizeof(uint32_t));
    memcpy(choclo+sizeof(uint32_t), &pcb->dl_tareas, sizeof(uint32_t));

    return choclo;
}

PCB_t* deserializar_pcb(void* stream) {
    PCB_t* pcb = malloc(sizeof(PCB_t));
    if (pcb == NULL) return NULL;

    memcpy(&pcb->pid, stream, sizeof(uint32_t));
    memcpy(&pcb->dl_tareas, stream+sizeof(uint32_t), sizeof(uint32_t));

    return pcb;
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

TCB_t* deserializar_tcb(void* stream) {
    TCB_t* tcb = malloc(sizeof(TCB_t));
    if (tcb == NULL) return NULL;

    memcpy(&tcb->tid, stream, sizeof(uint32_t));
    memcpy(&tcb->estado, stream+sizeof(uint32_t), sizeof(char));
    memcpy(&tcb->pos_x, stream+sizeof(uint32_t)+sizeof(char), sizeof(uint32_t));
    memcpy(&tcb->pos_y, stream+sizeof(uint32_t)+sizeof(char)+sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&tcb->id_sig_tarea, stream+sizeof(uint32_t)+sizeof(char)+2*sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&tcb->dl_pcb, stream+sizeof(uint32_t)+sizeof(char)+3*sizeof(uint32_t), sizeof(uint32_t));

    return tcb;
}

void* serializar_string_tareas(char* string) {
    void* choclo = malloc(strlen(string)+1);
    memcpy(choclo, string, strlen(string)+1);
    return choclo; // equivalente a hacer (void*) string, pero bueno
}
