#include "../include/test_protocolo.h"

// Dos muy simples de ejemplo

void test1() {
	CU_ASSERT_TRUE(1);
}
void test2() {
	CU_ASSERT_FALSE(1);
}

// Uno posta
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

/////////

CU_TestInfo tests_protocolo[] = {
    { "Test 1 es verdadero", test1 },
    { "Test 1 es falso", test2 },
    { "Test raw tareas to list (1)", test_raw_tareas_to_list_1 },
    { "Test raw tareas to list (2)", test_raw_tareas_to_list_2 },
    CU_TEST_INFO_NULL
};