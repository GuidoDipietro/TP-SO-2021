#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "../shared/include/sockets.h"
#include "./include/console.h"

#define MODULENAME "DIS"

int main() {

	// main del discordiador

	////////////
	// prueba cliente //

	t_config* config = config_create("discordiador.config");

	t_log* logger = log_create("log.log", MODULENAME, 1, LOG_LEVEL_DEBUG);

	int conexion = crear_conexion(
			logger,
			"IMS",
			config_get_string_value(config,"IP_IMS"),
			config_get_string_value(config, "PUERTO_IMS")
	); // eclipse pinta cosas grises aca porque es una ******

	////////////

	menu_start();

	log_info(logger, "Saliendo de DIS");
	//frees
	config_destroy(config);
	log_destroy(logger);
	liberar_conexion(&conexion);

	return EXIT_SUCCESS;
}
