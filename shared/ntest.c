#include "include/protocolo.h"
#include <string.h>

t_list* raw_tareas_to_list(char* texto) {
    t_list* lista_tareas = list_create();

    char** tareas = string_split(texto, "\n");
    char** p_tareas = tareas;
    while (*p_tareas != NULL) {
        string_trim(p_tareas);

        // Campos
        t_posicion* pos = malloc(sizeof(t_posicion));
        uint16_t duracion;
        char* nombre;
        uint16_t param;

        char* header = malloc(100);
        sscanf(*p_tareas, "%[^;];%hhd;%hhd;%hd", header, &pos->x, &pos->y, &duracion);
        string_trim(&header);

        char** header_split = string_split(header, " ");
        nombre = header_split[0];
        param = header_split[1]? atoi(header_split[1]) : 0;

        // printf("%s %d;%d;%d;%d\n", nombre, param, pos->x, pos->y, duracion);

        t_tarea* tarea = tarea_create(nombre, param, pos, duracion, nombre);
        list_add(lista_tareas, (void*) tarea);

        free(*p_tareas);
        free(nombre);
        free(header);
        free(header_split[1]);
        free(header_split);
        free(pos);
        p_tareas++;
    }

    free(tareas);
    return lista_tareas;
}

void print_t_tarea(t_tarea* t) {
    printf("%s %d;%d;%d;%d;%d\n",
        t->nombre,
        t->param,
        t->pos->x,
        t->pos->y,
        t->duracion,
        t->tipo
    );
}

int main() {

    char* texto =
        "GENERAR_OXIGENO 12;2;3;5\r\n"
        "CONSUMIR_OXIGENO 120;23;1\r\n"
        "GENERAR_COMIDA 4;2;3;1\r\n"
        "GENERAR_BASURA 12;2;3;\r\n"
        "DESCARTAR_BASURA;3;1;7\r\n"
        "TAREA_INVENTADA 14;27;49;3\r\n"
        "OTRA_TRUCHA;6;6;6\0"
    ;

    t_list* tareas = raw_tareas_to_list(texto);

    list_iterate(tareas, (void*) print_t_tarea);

    list_destroy_and_destroy_elements(tareas, (void*) free_t_tarea);

    return 0;
}