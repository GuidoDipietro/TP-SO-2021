#include "../include/graphic.h"

NIVEL* among_nivel;

extern t_log* logger;

void iniciar_gui(char* name) {
    int err = nivel_gui_inicializar();
    if (err) return;
    among_nivel = nivel_crear(name);
    nivel_gui_dibujar(among_nivel);
}

void finalizar_gui() {
    nivel_destruir(among_nivel);
    nivel_gui_terminar();
}

int crear_tripulantes(uint32_t c_tripulantes, t_list* posiciones) {
    int err;
    static char camiseta = 49; // el # de cada tripulante en la GUI (48 + TID)
    for (int i = 0; i < c_tripulantes; i++) {
        t_posicion* pos = list_get(posiciones, i);
        err = personaje_crear(among_nivel, camiseta, pos->x, pos->y);
        camiseta++;
    }

    return err;
//  personaje_crear(nivel, 'P', 10, 10);
}

int mover_tripulante(uint32_t id_tripulante, t_posicion* posicion) {
    int err;

    err = item_mover(among_nivel, (char) id_tripulante+48, posicion->x, posicion->y);

    nivel_gui_dibujar(among_nivel);

    return err;
}

int expulsar_tripulante(uint32_t id_tripulante) {
    int err;

    err = item_borrar(among_nivel, (char) id_tripulante+48);

    nivel_gui_dibujar(among_nivel);

    return err;
}

void chequear_errores(int err) {
    if (err) {
        char* err_str = nivel_gui_string_error(err);
        log_error(logger, "NIVEL_GUI_STRING_ERROR: %s", err_str);
        free(err_str);
    }
}
