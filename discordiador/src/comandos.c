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
    uint8_t cantidad_tripulantes = atoi(args_arr[0]);

    if(args_arr[2] != NULL) {
        bool error = false;
        char** init_pos = string_split(args_arr[2], " ");
        uint8_t cant_args = string_split_len(init_pos);

        if(cant_args == 0) {
            // cantidad no puede ser 0 / esta mal inicializado
            error = true;
            printf("\nFormato invalido o cantidad debe ser mayor que 0.\n");
        } else if(cant_args > cantidad_tripulantes) {
            // hay mas posiciones que tripulantes inicializados
            error = true;
            printf("\nHay mas posiciones que tripulantes inicializados.\n");
        }

        if(error) {
            string_split_free(&args_arr);
            string_split_free(&init_pos);
            return;
        }

        lista_posiciones = extraer_posiciones(init_pos);
        string_split_free(&init_pos);
    } else
        lista_posiciones = list_create();

    // Agregamos los que faltan para que esten todos en 0
    while(list_size(lista_posiciones) != cantidad_tripulantes) {
        t_posicion* n = malloc(sizeof(t_posicion));
        n->x = 0;
        n->y = 0;
        list_add(lista_posiciones, n);
    }

    ////////////// ENVIO DE MENSAJE A MRH (y IMS prueba) ////////////////
    FILE* f_tareas = fopen(args_arr[1],"r");
    if (f_tareas==NULL) {
        log_error(main_log, "No se pudo abrir el archivo de tareas en %s", args_arr[1]);
    }

    else {
        // Necesitamos pasarle el contenido del archivo en bytes
        // Porque si no aparentemente no puede leerlo desde otro directorio
        // no se no preguntes
        size_t sz_s_tareas;
        void* s_tareas = serializar_contenido_archivo(&sz_s_tareas, f_tareas);
        fclose(f_tareas);

        bool envio_mrh = send_patota(
            *mi_ram_hq_fd,
            cantidad_tripulantes,
            s_tareas, sz_s_tareas,
            lista_posiciones
        );
        if (!envio_mrh)
            log_error(main_log, "El envio de INICIAR_PATOTA al MI-RAM-HQ fallo");

        // este no tiene que estar en el TP
        bool envio_ims = send_patota(
            *i_mongo_store_fd,
            cantidad_tripulantes,
            s_tareas, sz_s_tareas,
            lista_posiciones
        );
        if(!envio_ims)
            log_error(main_log, "El envio de la patota al I_MONGO_STORE fallo");

        free(s_tareas);
    }
    ////////////////////////////////////////////////

    string_split_free(&args_arr);
    list_destroy_and_destroy_elements(lista_posiciones, free_t_posicion);
}

void listar_tripulantes(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args != NULL) {
        mensaje_error_sin_args("LISTAR_TRIPULANTES");
        return;
    }

    printf("\nComando LISTAR_TRIPULANTES\n");
}

void expulsar_tripulante(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    char* args_arr = argumento_unico(args);

    if(args_arr == NULL) {
        mensaje_error_con_args("EXPULSAR_TRIPULANTE", "<int>");
        return;
    }

    printf("\nComando EXPULSAR_TRIPULANTES\n");
    free(args_arr);
}

void iniciar_planificacion(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args != NULL) {
        mensaje_error_sin_args("INICIAR_PLANIFICACION");
        return;
    }

    printf("\nComando INICIAR_PLANIFICACION\n");
}

void pausar_planificacion(char* args, int* i_mongo_store_fd, int* mi_ram_hq_fd) {
    if(args != NULL) {
        mensaje_error_sin_args("PAUSAR_PLANIFICACION");
        return;
    }

    printf("\nComando PAUSAR_PLANIFICACION\n");
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
