#include "../include/listar_tripulantes.h"

static char* print_status(t_status status) {
    if(status == NEW)
        return "NEW";
    else if(status == READY)
        return "READY";
    else if(status == EXEC)
        return "EXEC";
    else if(status == BLOCKED)
        return "BLOCK I/O";
    else if(status == EXIT)
        return "EXIT";
}

static void imprimir_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;

    // 8 espacios
    printf("\nTripulante: %3d        Patota: %3d        Status: %s", t->tid, t->pid, print_status(t->status));
}

void op_listar_tripulantes() {
    printf("\n------------------------------");
    printf("\nEstado de la nave al %s", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
    iterar_cola(imprimir_tripulante);
    printf("\n------------------------------");
}