#include "../include/comandos.h"
#include "../../shared/include/utils.h"

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

static bool solo_numeros(char* str) {
    uint8_t i = 0;
    while(str[i] != '\0') {
        if(str[i] < 48 || str[i] > 57)
            return false;
        i++;
    }
    return true;
}

//
// Comandos
//

// INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4
// Estos chequeos podrian ponerse en otra funcion. Por ahora quedan aca.
void iniciar_patota(char *args) {
    if(args == NULL) {
        mensaje_error_con_args("INICIAR_PATOTA", "<int> <string> {<int>|<int>}*");
        return;
    }

    char** args_arr = string_split(args, " ");

    // Contamos cuantos argumentos hay
    uint8_t i = string_split_len(args_arr);

    uint8_t cantidad;
    bool error = false;

    if(i < 2) {
        // No hay suficientes argumentos
        error = true;
        mensaje_error_con_args("INICIAR_PATOTA", "<int> <string> {<int>|<int>}*");
    }

    if(!error) {
        cantidad = atoi(args_arr[0]);

        if(cantidad == 0) {
            // cantidad no puede ser 0 / esta mal inicializado
            error = true;
            printf("\nFormato invalido o cantidad debe ser mayor que 0.\n");
        } else if(i > cantidad + 2) {
            // hay mas posiciones que tripulantes inicializados
            error = true;
            printf("\nHay mas posiciones que tripulantes inicializados.\n");
        }
    }

    if(error) {
        string_split_free(&args_arr);
        return;
    }

    // Ahora pasamos a asignar las posiciones de cada uno de los tripulantes inicializados

    posicionTripulante posiciones[cantidad];
    // Inicializamos en 0
    for(int j = 0; j < cantidad; j++) {
        posiciones[j].x = 0;
        posiciones[j].y = 0;
    }

    for(int j = 2; j < i; j++) {
        char** pos_arr = string_split(args_arr[j], "|");

        if(string_split_len(pos_arr) != 2) {
            printf("\nFormato incorrecto\n");
            error = true;
        } else if(!(solo_numeros(pos_arr[0]) && solo_numeros(pos_arr[1]))) {
            printf("\nLa posicion del tripulante debe estar compuesta de numeros.\n");
            error = true;
        }

        if(error) {
            string_split_free(&args_arr);
            string_split_free(&pos_arr);
            return;
        }

        posiciones[j - 2].x = atoi(pos_arr[0]);
        posiciones[j - 2].y = atoi(pos_arr[1]);
        string_split_free(&pos_arr);
    }

    // Imprimimos cosas. Borrar esto

    printf("\nComando INICIAR_PATOTA\n");
    printf("%d - %s", cantidad, args_arr[1]);

    for(int j = 0; j < cantidad; j++)
        printf("\n %d | %d", posiciones[j].x, posiciones[j].y);
    printf("\n");

    // Fin impresion

    string_split_free(&args_arr);
}

void listar_tripulantes(char* args) {
    if(args != NULL) {
        mensaje_error_sin_args("LISTAR_TRIPULANTES");
        return;
    }

    printf("\nComando LISTAR_TRIPULANTES\n");
}

void expulsar_tripulante(char* args) {
    char* args_arr = argumento_unico(args);

    if(args_arr == NULL) {
        mensaje_error_con_args("EXPULSAR_TRIPULANTE", "<int>");
        return;
    }

    printf("\nComando EXPULSAR_TRIPULANTES\n");
    free(args_arr);
}

void iniciar_planificacion(char* args) {
    if(args != NULL) {
        mensaje_error_sin_args("INICIAR_PLANIFICACION");
        return;
    }

    printf("\nComando INICIAR_PLANIFICACION\n");
}

void pausar_planificacion(char* args) {
    if(args != NULL) {
        mensaje_error_sin_args("PAUSAR_PLANIFICACION");
        return;
    }

    printf("\nComando PAUSAR_PLANIFICACION\n");
}

void obtener_bitacora(char* args) {
    char* args_arr = argumento_unico(args);

    if(args_arr == NULL) {
        mensaje_error_con_args("OBTENER_BITACORA", "<int>");
        return;
    }

    printf("\nComando OBTENER_BITACORA\n");
    free(args_arr);
}