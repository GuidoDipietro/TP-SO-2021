#include "../include/test_utils.h"

// Tests para probar una util
void test_raw_tareas_to_list_1() {
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

    puts("\nTexto original:");
    for (int i=0; texto[i]!='\0'; putchar(texto[i++]));
    puts("\nContenido del t_list*:");
    list_iterate(tareas, (void*) print_t_tarea);

    list_destroy_and_destroy_elements(tareas, (void*) free_t_tarea);
}

void test_raw_tareas_to_list_2() {
    char* texto =
        "GENERAR_BASURA 12;2;3;\r\n"
        "DESCARTAR_BASURA;3;1;7\r\n"
        "TAREA_QUE_DEBERIA_ROMPER_COSAS\r\n"
        "TAREA_INVENTADA 14;27;49;3\r\n"
        "OTRA_ROMPEDORA 3;;\r\n"
        "UNA_TAREA_RARISIMA;;;;;;;\r\n"
        "OTRA_TRUCHA;6;6;6\0"
    ;

    t_list* tareas = raw_tareas_to_list(texto);

    puts("\n\nTexto original:");
    for (int i=0; texto[i]!='\0'; putchar(texto[i++]));
    puts("\n\nContenido del t_list*:");
    list_iterate(tareas, (void*) print_t_tarea);

    list_destroy_and_destroy_elements(tareas, (void*) free_t_tarea);
}

void test_asi_trimmeamos_las_tareas() {
    char* texto =
        "GENERAR_OXIGENO 12;2;3;5\r\n"
        "CONSUMIR_OXIGENO 120;23;1\r\n"
        "GENERAR_COMIDA 4;2;3;1\r\n"
        "GENERAR_BASURA 12;2;3;\r\n"
        "DESCARTAR_BASURA;3;1;7\r\n"
        "TAREA_INVENTADA 14;27;49;3\r\n"
        "OTRA_TRUCHA;6;6;6\0"
    ;

    char** listita = string_split(texto, "\n");

    char** p_listita = listita;
    for (; *p_listita != NULL; p_listita++) {
        string_trim(p_listita);
        printf("<<%s>>\n", *p_listita);
        free(*p_listita);
    }

    free(listita);
}

void test_string_tarea_to_t_tarea() {
    char* texto =
        "GENERAR_OXIGENO 12;2;3;5\n"
        "CONSUMIR_OXIGENO 120;23;1\n"
        "GENERAR_COMIDA 4;2;3;1\n"
        "GENERAR_BASURA 12;2;3;\n"
        "DESCARTAR_BASURA;3;1;7\n"
        "TAREA_INVENTADA 14;27;49;3\n"
        "OTRA_TRUCHA;6;6;6\0"
    ;
    char** string_tareas = string_split(texto, "\n");
    char** p_string_tareas = string_tareas;

    puts("");
    for (; *p_string_tareas != NULL; p_string_tareas++) {
        t_tarea* tarea = tarea_string_to_t_tarea(*p_string_tareas);
        printf("RAW:\t<<%s>>\n", *p_string_tareas);
        printf("PTR:\t"); print_t_tarea(tarea);
        puts("");
        free_t_tarea(tarea);
    }

    string_split_free(&string_tareas);
}

/////////

CU_TestInfo tests_utils[] = {
    { "Test raw tareas to list (1)", test_raw_tareas_to_list_1 },
    { "Test raw tareas to list (2)", test_raw_tareas_to_list_2 },
    { "Test trim tareas", test_asi_trimmeamos_las_tareas },
    { "Test string tarea to t_tarea*", test_string_tarea_to_t_tarea },
    CU_TEST_INFO_NULL,
};