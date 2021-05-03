#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "../shared/include/sockets.h"
#include "./include/tools.h"

#define MODULENAME "MRH"
#define SERVERNAME "MRH_SERVER"

int main() {

	// main del mi-ram-hq
	char* puerto;
	char* ip;

	// **** INICIALIZO EL CONFIG ****
	t_config* config = config_create("mi-ram-hq.config");

	// **** INICIALIZO EL LOGGER ****
	t_log* logger = log_create("log.log", MODULENAME, 1, LOG_LEVEL_DEBUG);

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

	// **** CONEXIONES ENTRANTES ****
	int conexion_entrante = esperar_cliente(logger, SERVERNAME, server_mrh);

	// **** LIBERO CONEXION ****
	liberar_conexion(&server_mrh);

	return EXIT_SUCCESS;
}
