#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "../shared/include/sockets.h"

#define MODULENAME "IMS"
#define SERVERNAME "IMS_SERVER"

int main() {

	// main del i_mongo_store

	/////// CONFIG ///////

	t_config* config = config_create("i_mongo_store.config");

	/////// CREACION DE SERVER ///////

	t_log* logger = log_create("log.log", MODULENAME, 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor(
			logger,
			SERVERNAME,
			config_get_string_value(config, "IP"),
			config_get_string_value(config, "PUERTO")
	);
	log_info(logger, "Server listo en IMS");

	/////// ENVIO-RECEPCION INDEFINIDA ///////

	// Envio y recepcion de mensajes perenne
	while (1) {
		int cliente_fd = esperar_cliente(logger, SERVERNAME, server_fd);

		// Mientras la conexion este abierta
		uint8_t prueba=0;
		while (cliente_fd != -1) {
			if (recv(cliente_fd, &prueba, sizeof(uint8_t), MSG_WAITALL) == 0)
				break;
			//TODO: envio y recepcion de mensajes. Falta definir protocolo.
			//Lo que esta puesto es a modo de prueba. Borrarlo.
		}

		// Cliente se va
		char msg[80];
		sprintf(msg, "Cliente desconectado de %s. Esperando otro cliente...", SERVERNAME);
		log_warning(logger, msg);
		// PRUEBA
		break; // ESTO SE VA A IR, ESTA A MODO DE PRUEBA
		// ENDPRUEBA
	}

	// si algun dia llega aca, que haga frees
	log_destroy(logger);
	config_destroy(config);
	return EXIT_SUCCESS;
}
