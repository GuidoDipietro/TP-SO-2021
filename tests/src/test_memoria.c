#include "../include/test_memoria.h"

void iniciar() {
    pthread_mutex_init(&MUTEX_SEGMENTOS_LIBRES, NULL);
}

void finalizar() {
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_LIBRES);
}

///

void prueba() {

    CU_ASSERT_EQUAL(NULL, NULL);
}

CU_TestInfo tests_memoria[] = {
    { "Test que compila", prueba },
    CU_TEST_INFO_NULL
};