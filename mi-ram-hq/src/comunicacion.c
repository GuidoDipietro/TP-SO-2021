#include "../include/comunicacion.h"

extern NIVEL* among_nivel;

extern t_log* logger;
extern t_config_mrhq* cfg;
extern t_list* segmentos_libres;
extern uint32_t memoria_disponible;

extern pthread_mutex_t MUTEX_MP_BUSY;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)
#define STR(_) #_

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
            log_info(logger, STR(DISCONNECT!));
            return;
        }

        switch (cop) {
            case DEBUG:
                log_info(logger, "Me llego el debug!");
                break;

            case INICIAR_PATOTA:
            {
                uint32_t n_tripulantes;
                char* tareas;
                t_list* posiciones;
                if (recv_patota(cliente_socket, &n_tripulantes, &tareas, &posiciones)) {
                    // MP

                    // Tengo que guardar:
                    // PCB          - 8 bytes           -> ahora
                    // Tareas       - N bytes           -> ahora
                    // Tripulantes  - 21 bytes cada uno -> cuando el tripulante avise

                    // Si aunque compactemos no entra, denegar
                    if (!entra_en_mp(8+strlen(tareas)+1+21*n_tripulantes)) {
                        log_error(logger, "No hay lugar para la patota en memoria");
                        send_ack(cliente_socket, false);

                        list_destroy_and_destroy_elements(posiciones, (void*) free);
                        free(tareas);
                        break;
                    }

                    send_ack(cliente_socket, true);

                    // Carga TAREAS, genera y carga PCB
                    // Carga la tabla de segmentos de la patota en la estruct. admin.
                    pthread_mutex_lock(&MUTEX_MP_BUSY);
                    // La compactacion sucede aca, de ser necesaria
                    bool ret_code = iniciar_patota_en_mp(n_tripulantes, tareas, posiciones);
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);

                    if (!ret_code) {
                        log_error(logger, "Error terrible iniciando patota en MRH");
                        list_destroy_and_destroy_elements(posiciones, (void*) free);
                        free(tareas);
                        break;
                    }

                    // GUI
                    int err = crear_tripulantes(n_tripulantes, posiciones);
                    chequear_errores(err);
                    nivel_gui_dibujar(among_nivel);

                    list_destroy(posiciones);
                    free(tareas);
                }
                else {
                    log_error(logger, "Error recibiendo patota en MRH");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case INICIAR_SELF_EN_PATOTA:
            {
                uint32_t id_tripulante;
                uint32_t id_patota;
                if (recv_iniciar_self_en_patota(cliente_socket, &id_tripulante, &id_patota)) {
                    // Crea y guarda en memoria el TCB
                    pthread_mutex_lock(&MUTEX_MP_BUSY);
                    bool ret_code = iniciar_tripulante_en_mp(id_tripulante, id_patota);
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);

                    if (!ret_code) {
                        log_error(logger,
                            "Error iniciando tripulante %" PRIu32 ", patota %" PRIu32 "\n",
                            id_tripulante, id_patota
                        );
                        break;
                    }
                }
                else {
                    log_error(logger, "Error recibiendo INICIAR_SELF_EN_PATOTA en MRH");
                }
                break;
            }

            case MOVIMIENTO:
            {
                uint32_t id_tripulante;
                t_posicion *origen, *destino;
                if (recv_movimiento(cliente_socket, &id_tripulante, &origen, &destino)) {
                    // MP
                    if (!actualizar_posicion_tripulante_en_mp(id_tripulante, destino)) {
                        log_error(logger, "Fallo actualizando posicion de tripulante en mp");
                        free(origen);
                        free(destino);
                        break;
                    }

                    // GUI
                    int err = mover_tripulante(id_tripulante, destino);
                    chequear_errores(err);

                    log_info(logger, "Se movio al %d de %d|%d a %d|%d",
                        id_tripulante, origen->x, origen->y, destino->x, destino->y
                    );
                    free_t_posicion(origen);
                    free_t_posicion(destino);
                }
                else {
                    log_error(logger, "Error recibiendo movimiento");
                }
                break;
            }

            case EXPULSAR_TRIPULANTE:
            {
                uint32_t id_tripulante;
                if (recv_tripulante(cliente_socket, &id_tripulante)) {
                    // MP
                    pthread_mutex_lock(&MUTEX_MP_BUSY);
                    bool ret_code = borrar_tripulante_de_mp(id_tripulante);
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);

                    if (!ret_code) {
                        log_error(logger,
                            "Error borrando tripulante %" PRIu32 "\n",
                            id_tripulante
                        );
                        break;
                    }

                    // GUI
                    int err = expulsar_tripulante(id_tripulante);
                    chequear_errores(err);
                }
                else {
                    log_error(logger, "Error recibiendo EXPULSAR_TRIPULANTE");
                }
                break;
            }
            case SOLICITAR_TAREA:
            {
                uint32_t tid;
                if (!recv_uint32_t(cliente_socket, &tid)) {
                    log_error(logger, "Error recibiendo TID en tarea solicitada");
                }
                else {
                    // log_info(logger, "Tripulante TID#%" PRIu32 " pide tarea.", tid);

                    t_tarea* tarea = fetch_tarea(tid);

                    if (!send_tarea(cliente_socket, tarea)) {
                        log_error(logger, "Error enviando la tarea a TID#%" PRIu32, tid);
                    }
                    free_t_tarea(tarea);
                }
                break;
            }
            case CAMBIO_ESTADO:
            {
                // TODO: modificar estado de tripulante en MP
                uint32_t id_tripulante;
                t_status estado;
                if (recv_cambio_estado(cliente_socket, &id_tripulante, &estado)) {
                    if (!actualizar_estado_tripulante_en_mp(id_tripulante, estado)) {
                        log_error(logger,
                            "Error actualizando estado de tripulante TID#%" PRIu32 "en MP a %c",
                            id_tripulante, estado
                        );
                    }
                }
                else {
                    log_error(logger, "Error recibiendo cambio de estado");
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

int server_escuchar(char* server_name, int server_socket) {
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
