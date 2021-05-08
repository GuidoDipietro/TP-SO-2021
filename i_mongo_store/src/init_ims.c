#include "../include/init_ims.h"

uint8_t cargar_configuracion(t_config_ims* config, t_log* log) {
    t_config* cfg = config_create("i_mongo_store.config");

    if(cfg == NULL) {
        log_error(log, "No se encontro i_mongo_store.config");
        return 0;
    }

    char* properties[] = {
            "PUNTO_MONTAJE",
            "PUERTO",
            "TIEMPO_SINCRONIZACION",
            "POSICIONES_SABOTAJE",
            NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg, properties)) {
        log_error(log, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg);
        return 0;
    }

    config->PUNTO_MONTAJE = strdup(config_get_string_value(cfg, "PUNTO_MONTAJE"));
    config->PUERTO = config_get_int_value(cfg, "PUERTO");
    config->TIEMPO_SINCRONIZACION = config_get_long_value(cfg, "TIEMPO_SINCRONIZACION");

    char** posiciones_sabotaje = config_get_array_value(cfg, "POSICIONES_SABOTAJE");
    config->POSICIONES_SABOTAJE = extraer_posiciones(posiciones_sabotaje);
    config_free_array_value(&posiciones_sabotaje);

    log_info(log, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg);

    return 1;
}

bool crear_servidor(int* fd, char* name, t_config_ims* cfg, t_log* logger) {
    *fd = iniciar_servidor(
            logger,
            name,
            "127.0.0.1",
            string_itoa(cfg->PUERTO)
    );
    log_info(logger, "Server listo en IMS");
    return (*fd != -1);
}

void server_escuchar(t_log* logger, char* server_name, int server_fd) {
    int cliente_fd = esperar_cliente(logger, server_name, server_fd);

    // Mientras la conexion este abierta
    op_code cop;
    while (cliente_fd != -1) {
        if (recv(cliente_fd, &cop, sizeof(op_code), 0) == 0)
            break;

        switch (cop) {
            // ESTO ES DEL MRH, ESTA ACA POR PRUEBAS TEMPORALMENTE
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
                break;
            // FIN COSAS DEL MRH
            case OBTENER_BITACORA:
                break;
            case MOVIMIENTO:
            case INICIO_TAREA:
            case FIN_TAREA:
            case ATENCION_SABOTAJE:
            case RESOLUCION_SABOTAJE:
            case GENERAR:
            case CONSUMIR:
            case DESCARTAR_BASURA:
            case INICIO_FSCK:
                break;
            case -1:
                log_info(logger, "cliente desconectado...");
                break;
            default:
                log_error(logger, "Algo anduvo mal en el server de IMS (que le mandaron?)");
                return;
        }
    }

    // Cliente se va
    log_warning(logger, "Cliente desconectado de %s. Esperando otro cliente...", server_name);
}

void cerrar_programa(t_log* log, t_config_ims* cfg) {
    log_destroy(log);

    if(cfg->PUNTO_MONTAJE != NULL)
        free(cfg->PUNTO_MONTAJE);

    if(cfg->POSICIONES_SABOTAJE != NULL)
        list_destroy_and_destroy_elements(cfg->POSICIONES_SABOTAJE, free_t_posicion);

    free(cfg);
}
