#include "../include/test_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;
extern t_list* segmentos_libres;
extern t_list* segmentos_usados;
extern uint32_t memoria_disponible;
extern void* memoria_principal;
extern segmento_t* (*proximo_hueco)(uint32_t);

void iniciar() {
    init();
    cargar_configuracion("src/test.config");
    cargar_memoria();
}

void finalizar() {
    cerrar_programa();
}

///

void test_print() {
    // Creanlo o no, este test me ayudo a encontrar un bug bastante feito
    print_seglib();
    print_segus();
}

void test_hueco_first_fit() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_ff = proximo_hueco_first_fit(4);
    CU_ASSERT_TRUE(
        hueco_ff->inicio == 10 && hueco_ff->tamanio == 5
    );
}

void test_hueco_first_fit_no_hay() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_ff = proximo_hueco_first_fit(14);
    CU_ASSERT_TRUE(hueco_ff == NULL);
}

void test_hueco_best_fit1() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_bf = proximo_hueco_best_fit(4);
    CU_ASSERT_TRUE(
        hueco_bf->inicio == 50 && hueco_bf->tamanio == 4
    );
}

void test_hueco_best_fit2() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 8);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 8);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_bf = proximo_hueco_best_fit(7);
    CU_ASSERT_TRUE(
        hueco_bf->inicio == 50 && hueco_bf->tamanio==8
    );
}

void test_hueco_best_fit_no_hay() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    segmento_t* hueco_bf = proximo_hueco_best_fit(27);
    CU_ASSERT_TRUE(hueco_bf == NULL);
}

void test_compactacion() {
    // Falta operar sobre void* memoria_principal
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);
    memoria_disponible -= (cfg->TAMANIO_MEMORIA-(2+5+10+4));
    // ^ esto se iria actualizando cada vez que se mete un segmento
    // WE LIVE IN A SIMULATION
    // https://www.youtube.com/watch?v=tLOIBw4uzpc

    puts("\n\nANTES DE COMPACTAR... TRAIGAN LA PRENSA HIDRAULICA");
    print_seglib();

    compactar_segmentos_libres();

    puts("DESPUES DE COMPACTAR (OUCH)");
    print_seglib();
}

void test_meter_segmento_ocupa_hueco_entero_bf() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    puts("\n\nANTES DE METER UN SEGMENTO DE TAMANIO 4:");
    print_seglib();

    uint32_t tamanio_segmento = 4;
    segmento_t* hueco_target = proximo_hueco_best_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib();
}

void test_meter_segmento_ocupa_hueco_entero_ff() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    puts("\n\nANTES DE METER UN SEGMENTO DE TAMANIO 5:");
    print_seglib();

    uint32_t tamanio_segmento = 5;
    segmento_t* hueco_target = proximo_hueco_first_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib();
}

void test_meter_segmento_bf() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    puts("\n\nANTES DE METER UN SEGMENTO DE TAMANIO 3:");
    print_seglib();

    uint32_t tamanio_segmento = 3;
    segmento_t* hueco_target = proximo_hueco_best_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib();
}

void test_meter_segmento_ff() {
    asesinar_seglib();
    segmentos_libres = list_create();
    
    segmento_t* hueco_1 = new_segmento(69, 0, 2);
    segmento_t* hueco_2 = new_segmento(69, 10, 5);
    segmento_t* hueco_3 = new_segmento(69, 17, 10);
    segmento_t* hueco_4 = new_segmento(69, 50, 4);
    list_add_seglib(hueco_1);
    list_add_seglib(hueco_2);
    list_add_seglib(hueco_3);
    list_add_seglib(hueco_4);

    puts("\n\nANTES DE METER UN SEGMENTO DE TAMANIO 3:");
    print_seglib();

    uint32_t tamanio_segmento = 3;
    segmento_t* hueco_target = proximo_hueco_first_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib();
}

void test_meter_segmento_en_mp_ff() {
    free(cfg->CRITERIO_SELECCION);
    cfg->CRITERIO_SELECCION = strdup("FF");
    proximo_hueco = &proximo_hueco_first_fit;
    // Segmento de tamanio 50
    void* data1 = malloc(50);
    memset(data1, 0x11, 50);
    // Segmento de tamanio 10
    void* data2 = malloc(10);
    memset(data2, 0x88, 10);

    puts("Dump primeros 65 bytes MP ANTES:");
    mem_hexdump(memoria_principal, 65);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50));
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10));

    puts("Dump primeros 65 bytes MP DESPUES:");
    mem_hexdump(memoria_principal, 65);

    print_seglib();
    print_segus();

    free(data1);
    free(data2);
}

void test_meter_segmento_en_mp_bf() {
    free(cfg->CRITERIO_SELECCION);
    cfg->CRITERIO_SELECCION = strdup("BF");
    proximo_hueco = &proximo_hueco_best_fit;
    // Segmento de tamanio 500
    void* data1 = malloc(50);
    memset(data1, 0x11, 50);
    // Segmento de tamanio 100
    void* data2 = malloc(10);
    memset(data2, 0x88, 10);

    puts("Dump primeros 65 bytes MP ANTES:");
    mem_hexdump(memoria_principal, 65);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50));
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10));

    puts("Dump primeros 65 bytes MP DESPUES:");
    mem_hexdump(memoria_principal, 65);

    print_seglib();
    __asm__ __volatile__ ("call print_segus");

    free(data1);
    free(data2);
}

void test_realloc_bf1() {
    proximo_hueco = &proximo_hueco_best_fit;
    void* data1 = malloc(50);
    memset(data1, 0x11, 50);
    void* data2 = malloc(10);
    memset(data2, 0x77, 10);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50));
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10));
    CU_ASSERT_TRUE(realloc_segmento_en_mp(50, 60));

    void* data3 = malloc(5);
    memset(data3, 0x44, 5);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data3, 5));

    print_seglib();
    print_segus();
    mem_hexdump(memoria_principal, 70);

    free(data1); free(data2); free(data3);
}

void test_realloc_bf2() {
    proximo_hueco = &proximo_hueco_best_fit;
    void* data1 = malloc(20);
    memset(data1, 0x22, 20);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 20));
    CU_ASSERT_TRUE(realloc_segmento_en_mp(0, 10));
    CU_ASSERT_TRUE(realloc_segmento_en_mp(10, 40));

    //  0-40    : 0000 size 40
    // 40-60    : 2222 size 10

    void* data2 = malloc(25);
    memset(data2, 0x33, 25);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 25));

    //  0-25    : 3333 size 25
    // 25-40    : 0000 size 15
    // 40-60    : 2222 size 20

    void* data3 = malloc(20);
    memset(data3, 0x44, 20);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data3, 20));

    //  0-25    : 3333 size 25
    // 25-40    : 0000 size 15
    // 40-60    : 2222 size 20
    // 60-80    : 4444 size 20

    CU_ASSERT_TRUE(realloc_segmento_en_mp(60, 70));

    //  0-25    : 3333 size 25
    // 25-40    : 0000 size 15
    // 40-60    : 2222 size 20
    // 60-70    : 0000 size 10
    // 70-90    : 4444 size 20

    CU_ASSERT_TRUE(realloc_segmento_en_mp(70, 73));

    //  0-25    : 3333 size 25
    // 25-40    : 0000 size 15
    // 40-60    : 2222 size 20
    // 60-73    : 0000 size 13 -> best fit para 12
    // 73-103   : 4444 size 20

    void* data4 = malloc(12);
    memset(data4, 0x77, 12);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data4, 12));

    //  0-25    : 3333 size 25
    // 25-40    : 0000 size 15
    // 40-60    : 2222 size 20
    // 60-72    : 7777 size 12
    // 72-73    : 0000 size  1
    // 73-103   : 4444 size 20

    print_seglib();
    print_segus();
    mem_hexdump(memoria_principal, 70);

    free(data1); free(data2); free(data3); free(data4);
}

void test_realloc_ff() {
    proximo_hueco = &proximo_hueco_first_fit;
    // cachos
    void* data1 = malloc(20);
    memset(data1, 0x11, 20);
    void* data2 = malloc(10);
    memset(data2, 0x77, 10);
    void* data3 = malloc(15);
    memset(data3, 0x66, 15);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 20));
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10));
    CU_ASSERT_TRUE(meter_segmento_en_mp(data3, 15));

    //  0-20    : 1111 size 20
    // 20-30    : 7777 size 10
    // 30-45    : 6666 size 15

    CU_ASSERT_FALSE(realloc_segmento_en_mp(15, 25)); // ningun segmento empieza en 15!

    CU_ASSERT_TRUE(realloc_segmento_en_mp(30, 50));

    //  0-20    : 1111 size 20
    // 20-30    : 7777 size 10
    // 30-50    : 0000 size 20
    // 50-65    : 6666 size 15

    CU_ASSERT_TRUE(realloc_segmento_en_mp(20, 31));

    //  0-20    : 1111 size 20
    // 20-31    : 0000 size 11
    // 31-41    : 7777 size 10
    // 41-50    : 0000 size  9
    // 50-65    : 6666 size 15

    CU_ASSERT_TRUE(realloc_segmento_en_mp(31, 35));
    // equivalente a realloc_segmento_en_mp(20, 35);

    //  0-20    : 1111 size 20
    // 20-35    : 0000 size 15 -> first fit para size 11
    // 35-45    : 7777 size 10
    // 45-50    : 0000 size  5
    // 50-65    : 6666 size 15

    void* data4 = malloc(11);
    memset(data4, 0x22, 11);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data4, 11));

    //  0-20    : 1111 size 20
    // 20-31    : 2222 size 11
    // 31-34    : 0000 size  3
    // 35-45    : 7777 size 10
    // 45-50    : 0000 size  5
    // 50-65    : 6666 size 15

    print_seglib();
    print_segus();
    mem_hexdump(memoria_principal, 70);

    free(data1); free(data2); free(data3); free(data4);
    // Y aqui el programador se quito la vida
}

CU_TestInfo tests_memoria[] = {
    { "Test print seglib/segus", test_print },
    { "Test proximo hueco first fit", test_hueco_first_fit },
    { "Test proximo hueco first fit (no hay)", test_hueco_first_fit_no_hay },
    { "Test proximo hueco best fit (1)", test_hueco_best_fit1 },
    { "Test proximo hueco best fit (2)", test_hueco_best_fit2 },
    { "Test proximo hueco best fit (no hay)", test_hueco_best_fit_no_hay },
    { "Test meter nuevo segmento (best fit - justo)", test_meter_segmento_ocupa_hueco_entero_bf },
    { "Test meter nuevo segmento (first fit - justo)", test_meter_segmento_ocupa_hueco_entero_ff },
    { "Test meter nuevo segmento (best fit)", test_meter_segmento_bf },
    { "Test meter nuevo segmento (first fit)", test_meter_segmento_ff },
    { "Test COMPACTACION", test_compactacion },
    { "Test meter segmento en MP (first fit)", test_meter_segmento_en_mp_ff },
    { "Test meter segmento en MP (best fit)", test_meter_segmento_en_mp_bf },
    { "Test realloc (1) (best fit)", test_realloc_bf1 },
    { "Test realloc (2) (best fit)", test_realloc_bf2 },
    { "Test realloc (first fit)", test_realloc_ff },
    CU_TEST_INFO_NULL,
};