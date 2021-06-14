#include "../include/comunicacion.h"

extern t_log* logger;
extern t_list* segmentos_libres;
extern uint32_t memoria_disponible;

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case DEBUG:;
                log_info(logger, "Me llego el debug!");
                break;

            case INICIAR_PATOTA:
            {
                uint32_t n_tripulantes;
                char** tareas;
                t_list* posiciones;
                if (recv_patota(cliente_socket, &n_tripulantes, &tareas, &posiciones)) {
                    // log_info(logger, "iniciaron a una patota de %d tripulantes", n_tripulantes);
                    // log_info(logger, "tareas:\n%s\n", tareas);

                    int err = crear_tripulantes(n_tripulantes, posiciones);
                    chequear_errores(err);
                    nivel_gui_dibujar(among_nivel);
                }
                else
                    log_error(logger, "Error recibiendo patota en MRH");
                list_destroy_and_destroy_elements(posiciones, *free_t_posicion);
                string_split_free(&tareas); // en realidad hay que guardarlo en memoria
                break;
            }
            case INICIAR_SELF_EN_PATOTA:
                break;
            case MOVIMIENTO:
            {
                uint32_t id_tripulante;
                t_posicion *origen, *destino;
                if (!recv_movimiento(cliente_socket, &id_tripulante, &origen, &destino)) {
                    log_error(logger, "Error recibiendo movimiento");
                }

                int err = mover_tripulante(id_tripulante, destino);
                chequear_errores(err);

                log_info(logger, "Se movio al %d de %d|%d a %d|%d",
                    id_tripulante, origen->x, origen->y, destino->x, destino->y
                );
                free_t_posicion(origen);
                free_t_posicion(destino);
                break;
            }
            case EXPULSAR_TRIPULANTE:
            {
                uint32_t id_tripulante;
                 if (recv_tripulante(cliente_socket, &id_tripulante)) {
                    int err = expulsar_tripulante(id_tripulante);
                    chequear_errores(err);
                    // TODO: borrar tripulante de memoria
                 }
                break;
            }
            // Errores
            case -1:
                log_error(logger, "Cliente desconectado de MRH...");
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de MRH");
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(char* server_name, int server_socket){
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

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
