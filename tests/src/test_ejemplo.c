#include "../include/test_ejemplos.h"

// Un coso de estos tests_algo.c por cada "suite" de tests que se quieran probar

// Se agregan tantas funciones void nombre(void) como tests unitarios se quieran hacer
// Hagamos tres:

void dos_mas_dos_es_cuatro() {
    CU_ASSERT_EQUAL(2+2, 4);
}

void esta_cosa_rara_da_cinco() {
    // no importa por que da 5 pero da 5. solo gocen
    char p='/'/'/',a=';';
    for(;a<'?';p=-~p,a++);

    CU_ASSERT_EQUAL(p, 5);
}

// Se llena esta estrctura asi

CU_TestInfo tests_ejemplo[] = {
    { "Dos mas dos es cuatro?", dos_mas_dos_es_cuatro },
    { "La cosa rara da cinco", esta_cosa_rara_da_cinco },
    CU_TEST_INFO_NULL
};

// Y en el main.c se declara
// extern CU_TestInfo tests_ejemplo
// Y se agrega a suites[]

// Compilar y ejecutar main.c con
// make tests

// Limpiar con
// make clean

// saludos cordiales

