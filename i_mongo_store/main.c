#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "../shared/include/sockets.h"
#include "../shared/include/protocolo.h"

#define MODULENAME "IMS"
#define SERVERNAME "IMS_SERVER"

int main() {

    // main del i_mongo_store

    /////// CONFIG ///////

    t_config* config = config_create("i_mongo_store.config");

    /////// CREACION DE SERVER ///////

    t_log* logger = log_create("i_mongo_store.log", MODULENAME, 1, LOG_LEVEL_DEBUG);

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
        op_code cop;
        while (cliente_fd != -1) {
            if (recv(cliente_fd, &cop, sizeof(op_code), 0) == 0)
                break;

            switch (cop) {
                case EXPULSAR_TRIPULANTE:;
                    uint8_t tripulante;
                    if (recv_tripulante(cliente_fd, &tripulante))
                        log_info(logger, "expulsaron al tripulante %d", tripulante);
                    else
                        log_error(logger, "Error recibiendo tripulante");
                    break;
                case INICIAR_PATOTA:;
                    uint8_t n_tripulantes;
                    char* filepath;
                    t_list* posiciones;
                    if (recv_patota(cliente_fd, &n_tripulantes, &filepath, &posiciones)) {
                        log_info(logger, "iniciaron a la patota %d, tareas en %s", n_tripulantes, filepath);
                        list_iterate(posiciones, print_t_posicion);
                    }
                    else
                        log_error(logger, "Error recibiendo patota");
                    list_destroy_and_destroy_elements(posiciones, free_t_posicion);
                    free(filepath);
                    log_destroy(logger);
                    config_destroy(config);
                    return EXIT_SUCCESS;
                    break;
                case -1:
                    log_info(logger, "cliente desconectado...");
                    break;
                default:
                    log_error(logger, "Algo anduvo mal en el server de IMS (que le mandaron?)");
                    return EXIT_FAILURE;
            }
        }

        // Cliente se va
        log_warning(logger, "Cliente desconectado de %s. Esperando otro cliente...", SERVERNAME);
    }

    // si algun dia llega aca, que haga frees
    log_destroy(logger);
    config_destroy(config);
    return EXIT_SUCCESS;
}
