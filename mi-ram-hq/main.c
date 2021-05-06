#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "../shared/include/sockets.h"
#include "include/graphic.h"

#define MODULENAME "MRH"
#define SERVERNAME "MRH_SERVER"
#define NIVELNAME "AMONG-OS"

int main() {

	// main del mi-ram-hq
	char* puerto;
	char* ip;
	NIVEL* among_os_nivel;
	int tamanio_nivel[2];

	// **** INICIALIZO EL CONFIG ****
	t_config* config = config_create("mi-ram-hq.config");

	// **** INICIALIZO EL LOGGER ****
	t_log* logger = log_create("mi-ram-hq.log", MODULENAME, 0, LOG_LEVEL_DEBUG);

	// **** OBTENGO INFO DESDE LA CONFIG ****
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// **** INICIO SERVIDOR, RESERVO PUERTO Y ESPERO CONEXIONES ****
	int server_mrh = iniciar_servidor(
			logger,
			MODULENAME,
			ip,
			puerto
	);

	log_info(logger, "Servidor MRH inicializado");

	// **** PARTE GRAFICA ****
	among_os_nivel = iniciar_ventana(logger, NIVELNAME);
	nivel_gui_get_area_nivel(&tamanio_nivel[0], &tamanio_nivel[1]);


	// **** CONEXIONES ENTRANTES ****
	// **** PRUEBA DE DIBUJO EN VENTANA ****
	int conexion_entrante = esperar_cliente(logger, SERVERNAME, server_mrh);

	// PRUEBA PARA UNA CONEXION
	if(conexion_entrante != -1){
		personaje_crear(
				among_os_nivel,
				// ESTA INFO DEBERIA RECIBIRSE DESDE EL DISCORDIADOR (ID Y POSICION ACTUAL DE LOS TRIPULANTES)
				'A',
				posicion_random(tamanio_nivel[0]),
				posicion_random(tamanio_nivel[1])
		);

	}

	nivel_gui_dibujar(among_os_nivel);

	// PRUEBA DE 'EXPULSAR CONEXION' (ESTO DEBERIA ACTIVARSE CON LA EXPULSION DE UN TRIPULANTE
	// ESTO SOLO LO DEJO DE PRUEBA
	int key;
	noecho();
	key = getch();
	if(key == 'W' || key == 'w') {
		item_borrar(among_os_nivel, 'A');
	}

	printw("Presiona cualquier tecla para salir...");

	nivel_gui_dibujar(among_os_nivel);

	getch();

	// **** LIBERO CONEXION ****
	liberar_conexion(&server_mrh);

	nivel_gui_terminar();

	return EXIT_SUCCESS;
}
