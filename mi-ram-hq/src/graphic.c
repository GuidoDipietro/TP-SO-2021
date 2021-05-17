#include "../include/graphic.h"

void iniciar_gui(t_log* logger, char* name) {
    nivel_gui_inicializar();
    // log_info(logger, "Se inicializo correctamente la ventana de MRH");
    among_nivel = nivel_crear(name);
    nivel_gui_dibujar(among_nivel);
}

int crear_tripulantes(uint8_t c_tripulantes, t_list* posiciones) {
	int err;
    for(int i = 0; i < c_tripulantes; i++) {
        char c = 49 + i;
        t_posicion* pos = list_get(posiciones, i);
        err = personaje_crear(among_nivel, c, pos->x, pos->y);
    }

    return err;
//  personaje_crear(nivel, 'P', 10, 10);
}

int mover_tripulante(uint8_t id_tripulante,t_posicion* posicion){
	int err;
	// para funcion chequear_errores()
	err = item_mover(among_nivel,id_tripulante, posicion->x, posicion->y);

	nivel_gui_dibujar(among_nivel);

	return err;
}

int expulsar_tripulante(uint8_t id_tripulante){
	int err;

	// para funcion chequear_errores()
	err = item_borrar(among_nivel,id_tripulante);

	nivel_gui_dibujar(among_nivel);

	return err;
}

void chequear_errores(int err){
	if(err){
		printf("WARN: %s\n", nivel_gui_string_error(err));
	}
}




