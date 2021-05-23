#include "../include/init_mrhq.h"

uint8_t cargar_configuracion(t_config_mrhq* config, t_log* log) {
    t_config* cfg = config_create("mi-ram-hq.config");

    if(cfg == NULL) {
        log_error(log, "No se encontro mi-ram-hq.config");
        return 0;
    }

    char* properties[] = {
            "TAMANIO_MEMORIA",
            "ESQUEMA_MEMORIA",
            "TAMANIO_PAGINA",
            "TAMANIO_SWAP",
            "PATH_SWAP",
            "ALGORITMO_REEMPLAZO",
            "PUERTO",
            "IP",
            NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg, properties)) {
        log_error(log, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg);
        return 0;
    }

    config->TAMANIO_MEMORIA = config_get_int_value(cfg, "TAMANIO_MEMORIA");
    config->ESQUEMA_MEMORIA = strdup(config_get_string_value(cfg, "ESQUEMA_MEMORIA"));
    config->TAMANIO_PAGINA = config_get_int_value(cfg, "TAMANIO_PAGINA");
    config->TAMANIO_SWAP = config_get_int_value(cfg, "TAMANIO_SWAP");
    config->PATH_SWAP = strdup(config_get_string_value(cfg, "PATH_SWAP"));
    config->ALGORITMO_REEMPLAZO = strdup(config_get_string_value(cfg, "ALGORITMO_REEMPLAZO"));
    config->PUERTO = config_get_int_value(cfg, "PUERTO");
    config->IP = config_get_int_value(cfg, "IP");

    log_info(log, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg);

    return 1;
}

void cerrar_programa(t_config_mrhq* cfg, t_log* log) {
    log_destroy(log);

    if(cfg->ALGORITMO_REEMPLAZO != NULL)
        free(cfg->ALGORITMO_REEMPLAZO);

    if(cfg->PATH_SWAP != NULL)
        free(cfg->PATH_SWAP);

    if(cfg->ESQUEMA_MEMORIA != NULL)
        free(cfg->ESQUEMA_MEMORIA);

    free(cfg);
}

int server_escuchar(t_log* logger, char* server_name, int server_socket){
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    op_code cop;
    while(cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) == 0)
            break;

        switch (cop) {
            case INICIAR_PATOTA:;
                uint8_t n_tripulantes;
                // char* tareas;
                t_list* tareas;
                t_list* posiciones;
                if (recv_patota(cliente_socket, &n_tripulantes, &tareas, &posiciones)) {
                    // log_info(logger, "iniciaron a una patota de %d tripulantes", n_tripulantes);
                    // log_info(logger, "tareas:\n%s\n", tareas);

                	// Creo que deberia crear tripulante por tripulante con el mensaje INICIAR_TRIPULANTE
                	// INICIAR_TRIPULANTE lo haria el discordiador automaticamente cuando inicia la patota

                	int err = crear_tripulantes(n_tripulantes, posiciones);
                    chequear_errores(err);
                    nivel_gui_dibujar(among_nivel);
                }
                else
                    log_error(logger, "Error recibiendo patota en MRH");
                list_destroy_and_destroy_elements(posiciones, *free_t_posicion);
                list_destroy_and_destroy_elements(tareas, *free_t_posicion); // quizas no quieran hacerle free en un futuro (muy cercano!)
                free(tareas);
                break;

            case MOVIMIENTO:;
            	//uint8_t id_tripulante;
            	/*
            	if (recv_movimiento(cliente_socket,)){
            	    int err = mover_tripulante(id_tripulante);
            	    chequear_errores(err);
            	    // TODO: borrar tripulante de memoria
            	 }
				*/
            break;
            case EXPULSAR_TRIPULANTE:;
            	uint8_t id_tripulante;
            	 if (recv_tripulante(cliente_socket, &id_tripulante)){
            		 int err = expulsar_tripulante(id_tripulante);
            		 chequear_errores(err);
            		// TODO: borrar tripulante de memoria
            	 }
            break;
            case -1:
                log_error(logger, "Cliente desconectado de MRH...");
                // return 1;
                return 0; //por pruebas!
            default:
                log_error(logger, "Algo anduvo mal en el server de MRH");
                return 0;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    // return 1;
    return 0; // por pruebas!
}







