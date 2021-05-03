#include "../include/graphic.h"

NIVEL* iniciar_ventana(t_log* logger, char* name) {

	NIVEL* nivel;

	log_info(logger, "Se inicializo correctamente la ventana de MRH");

	clear();
	nivel_gui_inicializar();
	nivel = nivel_crear(name);

	return nivel;
}

int posicion_random(int max_range){
	return rand() % max_range;
}
