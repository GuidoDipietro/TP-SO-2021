#include "../include/comandos.h"

//
// Utilidades
//

// Devuelve el argumento o NULL si esta mal
static char* argumento_unico(char* args) {
    if(args == NULL)
        return NULL;

    char** aux = string_split(args, " ");
    // Va a tener que ser de la forma [str, NULL]

    // Contamos cuantos argumentos hay
    uint8_t i = string_split_len(aux);

    if(i != 1) {
        string_split_free(&aux);
        return NULL;
    } else {
        char* ret = aux[0];
        free(aux);
        return ret;
    }
}

static void mensaje_error_con_args(char* cmd, char* formato) {
    printf("\nEl formato del comando es incorrecto. El formato correcto es\n");
    printf("\n%s %s\n", cmd, formato);
}

static void mensaje_error_sin_args(char* cmd) {
    printf("\nEl comando %s no acepta argumentos\n", cmd);
}

//
// Comandos
//

// INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4
// Estos chequeos podrian ponerse en otra funcion. Por ahora quedan aca.
void iniciar_patota(char *args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args == NULL) {
        mensaje_error_con_args("INICIAR_PATOTA", "<int> <string> {<int>|<int>}*");
        return;
    }

    if(SABOTAJE_ACTIVO) {
        log_error(main_log, "SABOTAJE ACTIVO. No se puede crear una patota en este momento.");
        return;
    }

    // args_arr[0] -> Cantidad de tripulantes a inicializar
    // args_arr[1] -> Path al archivo de tareas
    // args_arr[2] -> String con las posiciones (se debe parsear)
    char** args_arr = string_n_split(args, 3, " ");
    uint8_t i = string_split_len(args_arr);

    if(i < 2) {
        // No hay suficientes argumentos
        mensaje_error_con_args("INICIAR_PATOTA", "<int> <string> {<int>|<int>}*");
        string_split_free(&args_arr);
        return;
    }

    t_list* lista_posiciones;
    uint32_t cantidad_tripulantes = atoi(args_arr[0]);

    if(args_arr[2] != NULL) {
        bool error = false;
        char** init_pos = string_split(args_arr[2], " ");
        uint8_t cant_args = string_split_len(init_pos);

        if(cant_args == 0) {
            // cantidad no puede ser 0 / esta mal inicializado
            error = true;
            printf("\nFormato invalido o cantidad debe ser mayor que 0.\n");
        } else if (cant_args > cantidad_tripulantes) {
            // hay mas posiciones que tripulantes inicializados
            error = true;
            printf("\nHay mas posiciones que tripulantes inicializados.\n");
        }

        if (error) {
            string_split_free(&args_arr);
            string_split_free(&init_pos);
            return;
        }

        lista_posiciones = extraer_posiciones(init_pos);
        string_split_free(&init_pos);
    } else
        lista_posiciones = list_create();

    // Agregamos los que faltan para que esten todos en 0
    while (list_size(lista_posiciones) != cantidad_tripulantes) {
        t_posicion* n = malloc(sizeof(t_posicion));
        n->x = 0;
        n->y = 0;
        list_add(lista_posiciones, n);
    }

    // Chequeos terminados. Pasamos a iniciar la patota
    // Envio del mensaje

    size_t sz_s_tareas;
    void* s_tareas = serializar_contenido_archivo(&sz_s_tareas, args_arr[1], main_log);

    if (s_tareas != NULL) {
        // Primero vamos a calcular cuantos tripulantes hay que crear
        char* text = leer_archivo_completo(args_arr[1]);

        t_list* tareas = raw_tareas_to_list(text);
        int16_t dif = cantidad_tripulantes - list_size(tareas);
        list_destroy_and_destroy_elements(tareas, free_t_tarea);
        free(text);

        // IMPORTANTE: Observar que por ahora no se le piden las tareas al Mi-Ram-HQ. Sincronizar la cantidad de tareas
        // con las que da la funcion solicitar_tarea()

        // Descomentar esto cuando se implemente el mi-ram-hq
        /*if (dif > 0) { // Hay mas tripulantes que tareas
            for (uint32_t k = cantidad_tripulantes - dif + 1; k <= cantidad_tripulantes; k++)
                log_warning(main_log, "No hay tarea para el tripulante %d. No va a ser creado.", k);

            cantidad_tripulantes -= dif;
        }*/
            
        bool envio_mrh = send_patota(
            *mi_ram_hq_fd,
            cantidad_tripulantes,
            s_tareas, sz_s_tareas,
            lista_posiciones
        );

        bool iniciar_patota_ack;
        recv_patota_ack(*mi_ram_hq_fd, &iniciar_patota_ack);

        if (!envio_mrh || !iniciar_patota_ack)
            log_error(main_log, "El envio de INICIAR_PATOTA al MI-RAM-HQ fallo");
        else {
            // Iniciamos cada tripulante
            uint32_t pid = generar_pid();
            for (uint32_t j = 0; j < cantidad_tripulantes; j++) {
                t_posicion* pos = malloc(sizeof(t_posicion));
                memcpy(pos, list_get(lista_posiciones, j), sizeof(t_posicion));

                t_running_thread* thread_data = malloc(sizeof(t_running_thread));
                thread_data->quantum = 0;
                thread_data->t = iniciar_tripulante(pos, pid);

                if(thread_data->t == NULL) { // Error creando el tripulante
                    free(thread_data);
                    continue;
                }

                thread_data->blocked = false;
                sem_init(&(thread_data->sem_pause), 0, 0);

                // Si la planificacion no esta bloqueada, el planificador lo acepta directamente
                if(PLANIFICACION_BLOQUEADA)
                    push_cola_new(thread_data);
                else {
                    (thread_data->t)->status = READY;
                    push_cola_tripulante(thread_data);
                }

                pthread_create(&(thread_data->thread), NULL, correr_tripulante, thread_data);
                pthread_detach(thread_data->thread);
            }
        }
        free(s_tareas);
    }

    // Free finales

    string_split_free(&args_arr);
    list_destroy_and_destroy_elements(lista_posiciones, free_t_posicion);
}

void listar_tripulantes(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args != NULL) {
        mensaje_error_sin_args("LISTAR_TRIPULANTES");
        return;
    }
    op_listar_tripulantes();
}

void expulsar_tripulante(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    char* args_arr = argumento_unico(args);

    if(args_arr == NULL) {
        mensaje_error_con_args("EXPULSAR_TRIPULANTE", "<int>");
        return;
    }

    uint8_t ret_code1 = op_expulsar_tripulante(atoi(args_arr));

    if(!ret_code1) {
        uint8_t ret_code2 = send_tripulante(*mi_ram_hq_fd, (uint32_t) atoi(args_arr), EXPULSAR_TRIPULANTE);

        if(ret_code2)
            log_info(main_log, "El tripulante %s fue expulsado", args_arr);
        else
            log_error(main_log, "No se pudo eliminar al tripulante %s", args_arr);
    }

    free(args_arr);
}

pthread_t thread_planificacion;
void iniciar_planificacion(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args != NULL) {
        mensaje_error_sin_args("INICIAR_PLANIFICACION");
        return;
    }

    if(SABOTAJE_ACTIVO) {
        log_error(main_log, "SABOTAJE ACTIVO. No se puede reanudar la planificacion.");
        return;
    }

    if(PLANIFICACION_BLOQUEADA)
        reanudar_planificacion();

    if(!PLANIFICADOR_ALIVE) {
        pthread_create(&thread_planificacion, NULL, (void*) planificador, NULL);
        pthread_detach(thread_planificacion);  
    }
}

void pausar_planificacion(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args != NULL) {
        mensaje_error_sin_args("PAUSAR_PLANIFICACION");
        return;
    }

    bloquear_planificacion();
}

void obtener_bitacora(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    char* args_arr = argumento_unico(args);

    if(args_arr == NULL) {
        mensaje_error_con_args("OBTENER_BITACORA", "<int>");
        return;
    }

    printf("\nComando OBTENER_BITACORA\n");
    free(args_arr);
}
