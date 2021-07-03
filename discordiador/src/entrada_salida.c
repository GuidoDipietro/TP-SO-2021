#include "../include/entrada_salida.h"

// Esto es basicamente un planificador, pero de e/s
void controlador_es() {
    while(1) {
        sem_wait(&TRIPULANTE_EN_BLOQUEADOS);
        t_running_thread* thread = pop_cola_bloqueados();

    } 
}

void tarea_io(t_running_thread* thread) {

}