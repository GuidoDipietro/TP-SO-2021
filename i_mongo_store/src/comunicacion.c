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
            case MOVIMIENTO: {
                uint32_t id_tripulante;
                t_posicion *origen, *destino;
                if (recv_movimiento(cliente_socket, &id_tripulante, &origen, &destino)) {
                    log_info(logger, "Recibido movimiento de tripulante.");
                    char* nombre = string_from_format("Tripulante%d.ims", id_tripulante);
                    char* content = string_from_format(
                        "Tripulante 1 se mueve de %d|%d a %d|%d\n",
                        id_tripulante,
                        origen->x, origen->y,
                        destino->x, destino->y
                    );
                    completar_bitacora(nombre, content, strlen(content));
                    free_t_posicion(origen);
                    free_t_posicion(destino);
                    free(nombre);
                    free(content);
                }
                else {
                    log_error(logger, "Error recibiendo movimiento");
                }
                break;
            }
            case INICIO_TAREA: {
                uint32_t id_tripulante;
                char* nombre_tarea;
                if(recv_tripulante_nombretarea(cliente_socket, &id_tripulante, &nombre_tarea)) {
                    log_info(logger, "Recibido inicio de tarea.");
                    char* nombre = string_from_format("Tripulante%d.ims", id_tripulante);
                    char* content = string_from_format(
                        "El tripulante %d inicia la tarea %s\n",
                        id_tripulante,
                        nombre_tarea
                    );
                    completar_bitacora(nombre, content, strlen(content));
                    free(nombre);
                    free(nombre_tarea);
                    free(content);
                } else
                    log_error(logger, "Error recibiendo el aviso de inicio de tarea");
                break;
            }
            case FIN_TAREA: {
                uint32_t id_tripulante;
                char* nombre_tarea;
                if(recv_tripulante_nombretarea(cliente_socket, &id_tripulante, &nombre_tarea)) {
                    log_info(logger, "Recibido fin de tarea.");
                    char* nombre = string_from_format("Tripulante%d.ims", id_tripulante);
                    char* content = string_from_format(
                        "El tripulante %d finaliza la tarea %s\n",
                        id_tripulante,
                        nombre_tarea
                    );
                    completar_bitacora(nombre, content, strlen(content));
                    free(nombre);
                    free(nombre_tarea);
                    free(content);
                } else
                    log_error(logger, "Error recibiendo el aviso de fin de tarea");
                break;
            }
            case ATENCION_SABOTAJE:
            case RESOLUCION_SABOTAJE:
            case GENERAR: {
                tipo_item tipo;
                uint16_t cantidad;
                if(recv_item_cantidad(cliente_socket, &tipo, &cantidad)) {
                    log_info(logger, "Tarea de generar recursos recibida");
                    tarea_generar(tipo, cantidad);
                } else{
                    log_error(logger, "Error fatal recibiendo instruccion de generar recursos");
                }
                break;
            }
                
            case CONSUMIR: {
                tipo_item tipo;
                uint16_t cantidad;
                if(recv_item_cantidad(cliente_socket, &tipo, &cantidad)) {
                    log_info(logger, "Tarea de consumir recursos recibida");
                    tarea_consumir(tipo, cantidad);
                } else{
                    log_error(logger, "Error fatal recibiendo instruccion de consumir recursos");
                }
                break;
            }

            case DESCARTAR_BASURA:
                descartar_basura();
                break;
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

/*

bool send_obtener_bitacora(int fd, uint32_t id_tripulante);
bool send_bitacora(int fd, char* bitacora);
bool recv_bitacora(int fd, char** bitacora);
bool send_sabotaje(int fd, t_posicion* posicion);
bool recv_sabotaje(int fd, t_posicion** posicion);
bool send_iniciar_fsck(int fd);
bool send_generar_consumir(int fd, tipo_item item, uint16_t cant, op_code cop);
bool recv_item_cantidad(int fd, tipo_item* item, uint16_t* cant);
bool send_descartar_basura(int fd); // solo op code

bool send_inicio_tarea(int fd, uint32_t id_tripulante, char* nombre_tarea);
bool send_fin_tarea(int fd, uint32_t id_tripulante, char* nombre_tarea);
bool recv_tripulante_nombretarea(int fd, uint32_t* id_tripulante, char** nombre_tarea);

*/