#include "../include/graphic.h"

NIVEL* iniciar_gui(t_log* logger, char* name) {

	NIVEL* nivel;
	nivel_gui_inicializar();
	// log_info(logger, "Se inicializo correctamente la ventana de MRH");
	nivel = nivel_crear(name);
	nivel_gui_dibujar(nivel);

	return nivel;
}

// Prueba
void obtener_posiciones(void* p) {
    posicion* t_p = (posicion*) p;
    uint8_t suma_posiciones = (t_p->x) + (t_p->y);
    printf("Suma posicion: %d\n", suma_posiciones);
}

// Si... esto estoy seguro de que debe haber una manera mucho mas sencillo de implementarlo
// Pero me volvi loco intentando hacerlo y son las 6:20 asi que lo voy a dejar como para tener
// Algo para mostrar hoy :)
void crear_tripulantes(NIVEL* nivel, uint8_t c_tripulantes, t_list* posiciones) {

	for(int i = 0; i < c_tripulantes; i++) {
		char c = 49 + i;
		posicion* pos = malloc(sizeof(posicion));
		pos = (posicion *)list_get(posiciones, i);
		personaje_crear(nivel, c, pos->x, pos->y);
		free(pos);
	}

//	personaje_crear(nivel, 'P', 10, 10);
}
