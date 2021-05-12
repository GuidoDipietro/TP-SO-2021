#include "../include/graphic.h"

NIVEL* iniciar_gui(t_log* logger, char* name) {

    NIVEL* nivel;
    nivel_gui_inicializar();
    // log_info(logger, "Se inicializo correctamente la ventana de MRH");
    nivel = nivel_crear(name);
    nivel_gui_dibujar(nivel);

    return nivel;
}

void crear_tripulantes(NIVEL* nivel, uint8_t c_tripulantes, t_list* posiciones) {

    for(int i = 0; i < c_tripulantes; i++) {
        char c = 49 + i;
        t_posicion* pos = list_get(posiciones, i);
        personaje_crear(nivel, c, pos->x, pos->y);
    }

//  personaje_crear(nivel, 'P', 10, 10);
}
