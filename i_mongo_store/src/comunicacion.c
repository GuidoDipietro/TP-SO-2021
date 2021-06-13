#include "../include/comunicacion.h"

extern t_log* logger;

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);

    // Mientras la conexion este abierta
    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) == 0) {
        	log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case DEBUG:;
                log_info(logger, "Me llego el debug!");
                break;
            case OBTENER_BITACORA:
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
            // Errores
            case -1:
                log_info(logger, "Cliente desconectado de IMS...");
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de IMS");
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(char* server_name, int server_fd) {
    int cliente_socket = esperar_cliente(logger, server_name, server_fd);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}