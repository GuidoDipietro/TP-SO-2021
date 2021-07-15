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
            case GENERAR: {
                tipo_item tipo;
                uint16_t cantidad;
                if(recv_item_cantidad(cliente_socket, &tipo, &cantidad)) {
                    log_info(logger, "Tarea de generar recursos recibida");
                    char c;
                    char* archivo;
                    if(tipo == OXIGENO) {
                        c = 'O';
                        archivo = "Oxigeno.ims";
                    } else if(tipo == COMIDA) {
                        c = 'C';
                        archivo = "Comida.ims";
                    } else if(tipo == BASURA) {
                        c = 'B';
                        archivo = "Basura.ims";
                    }
                    tarea_generar(archivo, c, cantidad);
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
                    char* archivo;
                    if(tipo == OXIGENO)
                        archivo = "Oxigeno.ims";
                    else if(tipo == COMIDA)
                        archivo = "Comida.ims";

                    tarea_consumir(archivo, cantidad);
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

*/