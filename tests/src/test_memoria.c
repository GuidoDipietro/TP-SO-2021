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
    print_segus();

    free(data1);
    free(data2);
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
    CU_TEST_INFO_NULL,
};