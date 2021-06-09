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
    else if(status == BLOCKEDSAB)
        return "BLOCK SAB";
    else if(status == EXIT)
        return "EXIT";
}

static void imprimir_tripulante(void* t_p) {
    t_tripulante* t = (t_tripulante*) t_p;

    // 8 espacios
    printf(
        "\nTripulante: %3d        Patota: %3d        Status: %s",
        t->tid, t->pid, print_status(t->status)
    );
}

// Para imprimir los tripulantes que se estan ejecutando
static void imprimir_tripulante_exec(void* t_p) {
    imprimir_tripulante(((t_running_thread*) t_p)->t);
}

void op_listar_tripulantes() {
    printf("\n------------------------------");
    char* timestamp = temporal_get_string_time("%d/%m/%y %H:%M:%S");
    printf("\nEstado de la nave al %s", timestamp);
    free(timestamp);
    iterar_lista_hilos(imprimir_tripulante_exec);
    iterar_cola_ready(imprimir_tripulante_exec);
    iterar_cola_new(imprimir_tripulante_exec);
    iterar_lista_exit(imprimir_tripulante);
    printf("\n------------------------------");
}