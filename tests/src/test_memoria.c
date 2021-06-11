#include "../include/test_memoria.h"

void iniciar() {
    segmentos_libres = list_create();
    pthread_mutex_init(&MUTEX_SEGMENTOS_LIBRES, NULL);
}

void finalizar() {
    pthread_mutex_destroy(&MUTEX_SEGMENTOS_LIBRES);
    asesinar_seglib();
}

///

void test_print_seglib() {
    segmento_t* hueco_1 = new_segmento(0, 10);
    segmento_t* hueco_2 = new_segmento(15, 7);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    
    print_seglib();
}

void test_hueco_first_fit() {
    segmento_t* hueco_1 = new_segmento(0, 2);
    segmento_t* hueco_2 = new_segmento(10, 5);
    segmento_t* hueco_3 = new_segmento(17, 10);
    segmento_t* hueco_4 = new_segmento(50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_ff = proximo_hueco_first_fit(4);
    CU_ASSERT_TRUE(
        hueco_ff->inicio == 10 && hueco_ff->tamanio == 5
    );
    free(hueco_ff);
}

void test_hueco_first_fit_no_hay() {
    segmento_t* hueco_1 = new_segmento(0, 2);
    segmento_t* hueco_2 = new_segmento(10, 5);
    segmento_t* hueco_3 = new_segmento(17, 10);
    segmento_t* hueco_4 = new_segmento(50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_ff = proximo_hueco_first_fit(14);
    CU_ASSERT_TRUE(hueco_ff == NULL);
}

void test_hueco_best_fit1() {
    segmento_t* hueco_1 = new_segmento(0, 2);
    segmento_t* hueco_2 = new_segmento(10, 5);
    segmento_t* hueco_3 = new_segmento(17, 10);
    segmento_t* hueco_4 = new_segmento(50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_bf = proximo_hueco_best_fit(4);
    CU_ASSERT_TRUE(
        hueco_bf->inicio == 50 && hueco_bf->tamanio == 4
    );
    free(hueco_bf);
}

void test_hueco_best_fit2() {
    segmento_t* hueco_1 = new_segmento(0, 2);
    segmento_t* hueco_2 = new_segmento(10, 8);
    segmento_t* hueco_3 = new_segmento(17, 10);
    segmento_t* hueco_4 = new_segmento(50, 8);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_bf = proximo_hueco_best_fit(7);
    CU_ASSERT_TRUE(
        hueco_bf->inicio == 50 && hueco_bf->tamanio==8
    );
    free(hueco_bf);
}

void test_hueco_best_fit_no_hay() {
    segmento_t* hueco_1 = new_segmento(0, 2);
    segmento_t* hueco_2 = new_segmento(10, 5);
    segmento_t* hueco_3 = new_segmento(17, 10);
    segmento_t* hueco_4 = new_segmento(50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_bf = proximo_hueco_best_fit(27);
    CU_ASSERT_TRUE(hueco_bf == NULL);
}

void test_compactacion() {
    segmento_t* hueco_1 = new_segmento(0, 2);
    segmento_t* hueco_2 = new_segmento(10, 5);
    segmento_t* hueco_3 = new_segmento(17, 10);
    segmento_t* hueco_4 = new_segmento(50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    puts("\n\nANTES DE COMPACTAR... TRAIGAN LA PRENSA HIDRAULICA");
    print_seglib();

    compactar_segmentos_libres();

    puts("DESPUES DE COMPACTAR (OUCH)");
    print_seglib();
}

CU_TestInfo tests_memoria[] = {
    { "Test print seglib", test_print_seglib },
    { "Test proximo hueco first fit", test_hueco_first_fit },
    { "Test proximo hueco first fit (no hay)", test_hueco_first_fit_no_hay },
    { "Test proximo hueco best fit (1)", test_hueco_best_fit1 },
    { "Test proximo hueco best fit (2)", test_hueco_best_fit2 },
    { "Test proximo hueco best fit (no hay)", test_hueco_best_fit_no_hay },
    { "Test COMPACTACION", test_compactacion },
    CU_TEST_INFO_NULL,
};