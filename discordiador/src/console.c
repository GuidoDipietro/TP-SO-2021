#include "../include/console.h"
#include "../include/comandos.h"
#include "../../shared/include/utils.h"

char** completion(const char*, int, int);
char* generator(const char*, int);
static void parse_command(char*);

static void no_implementado(char* args) {
    printf("\nComando no implementado\n");
}

// Este array es para la libreria readline
char* commands[] = {
        "INICIAR_PATOTA",
        "LISTAR_TRIPULANTES",
        "EXPULSAR_TRIPULANTE",
        "INICIAR_PLANIFICACION",
        "PAUSAR_PLANIFICACION",
        "OBTENER_BITACORA",
        NULL
};

commandMap cmdMap[] = {
        { iniciar_patota, "INICIAR_PATOTA" },
        { listar_tripulantes, "LISTAR_TRIPULANTES" },
        { expulsar_tripulante, "EXPULSAR_TRIPULANTE" },
        { iniciar_planificacion, "INICIAR_PLANIFICACION" },
        { pausar_planificacion, "PAUSAR_PLANIFICACION" },
        { obtener_bitacora, "OBTENER_BITACORA" },
        NULL
};

void menu_start() {
    rl_bind_key('\t', rl_complete);
    rl_attempted_completion_function = completion;

    char* input;
    bool isExit = false;

    do {
        input = readline("\n> ");
        string_trim(&input);
        add_history(input);

        if(strlen(input) == 5)
            isExit = (bool)(strcmp(input, "salir\0") == 0);

        // Si no hay que salir, parseamos el comando
        if(!isExit)
            parse_command(input);

        free(input);
    } while(!isExit);

    rl_clear_history();
}

void parse_command(char* string) {
    char** split_cmd = string_n_split(string, 2, " ");
    // Si no hay argumentos split_cmd[1] es NULL

    // Compara el comando con cada uno de los comandos creados en cmds[]
    for(uint8_t i = 0; cmdMap[i].cmdName != NULL; i++) {
        if(strcmp(split_cmd[0], cmdMap[i].cmdName) == 0) {
            (cmdMap[i].func)(split_cmd[1]);
            string_split_free(&split_cmd);
            return;
        }
    }

    printf("\nEl comando ingresado es incorrecto\n");
    string_split_free(&split_cmd);
}

// Estas funciones son para que funcionen bien la libreria readline

char** completion(const char* text, int start, int end) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, generator);
}

char* generator(const char* text, int state) {
    static int idx;
    char* ret;

    if(!state)
        idx = 0;

    while((ret = commands[idx++]))
        if(strstr(ret, text) != NULL)
            return strdup(ret);

    return NULL;
}