#include "../include/test_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;

extern t_list* segmentos_libres;
extern t_list* segmentos_usados;
extern t_list* ts_patotas;
extern t_list* ts_tripulantes;
extern segmento_t* (*proximo_hueco)(uint32_t);

extern t_list* tp_patotas;
extern frame_t* tabla_frames;

extern uint32_t memoria_disponible;
extern void* memoria_principal;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)
#define TEST_SEG 14

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
    print_seglib(false);
    print_segus(false);
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
    print_seglib(false);

    uint32_t tamanio_segmento = 4;
    segmento_t* hueco_target = proximo_hueco_best_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib(false);
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
    print_seglib(false);

    uint32_t tamanio_segmento = 5;
    segmento_t* hueco_target = proximo_hueco_first_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib(false);
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
    print_seglib(false);

    uint32_t tamanio_segmento = 3;
    segmento_t* hueco_target = proximo_hueco_best_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib(false);
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
    print_seglib(false);

    uint32_t tamanio_segmento = 3;
    segmento_t* hueco_target = proximo_hueco_first_fit(tamanio_segmento);
    CU_ASSERT_TRUE(
        meter_segmento_actualizar_hueco(hueco_target, tamanio_segmento)
    );

    puts("DESPUES DE ELLO");
    print_seglib(false);
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

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10, TEST_SEG) != INICIO_INVALIDO);

    puts("Dump primeros 65 bytes MP DESPUES:");
    mem_hexdump(memoria_principal, 65);

    print_seglib(false);
    print_segus(false);

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

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10, TEST_SEG) != INICIO_INVALIDO);

    puts("Dump primeros 65 bytes MP DESPUES:");
    mem_hexdump(memoria_principal, 65);

    print_seglib(false);
    print_segus(false);
    // __asm__ __volatile__ ("call print_segus");

    free(data1);
    free(data2);
}

void test_compactacion() {
    proximo_hueco = &proximo_hueco_first_fit;
    void* data1 = malloc(10);
    void* data2 = malloc( 5);
    void* data3 = malloc(20);
    void* data4 = malloc( 7);
    void* data5 = malloc(16);
    void* data6 = malloc(30);
    memset(data1, 0x11, 10);
    memset(data2, 0x22,  5);
    memset(data3, 0x33, 20);
    memset(data4, 0x44,  7);
    memset(data5, 0x55, 16);
    memset(data6, 0x66, 30);

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 10, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2,  5, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data3, 20, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data4,  7, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data5, 16, TEST_SEG) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data6, 30, TEST_SEG) != INICIO_INVALIDO);

    //  0-10    : 1111 size 10
    // 10-15    : 2222 size  5
    // 15-35    : 3333 size 20
    // 35-42    : 4444 size  7
    // 42-58    : 5555 size 16
    // 58-88    : 6666 size 30

    CU_ASSERT_TRUE(eliminar_segmento_de_mp(15));
    CU_ASSERT_FALSE(eliminar_segmento_de_mp(3)); // no existe segmento
    CU_ASSERT_TRUE(eliminar_segmento_de_mp(42));

    //  0-10    : 1111 size 10
    // 10-15    : 2222 size  5
    // 15-35    : 0000 size 20
    // 35-42    : 4444 size  7
    // 42-58    : 0000 size 16
    // 58-88    : 6666 size 30

    CU_ASSERT_TRUE(compactar_mp());

    //  0-10    : 1111 size 10
    // 10-15    : 2222 size  5
    // 15-22    : 4444 size  7
    // 22-52    : 6666 size 30

    print_seglib(false);
    print_segus(false);
    mem_hexdump(memoria_principal, 100);

    CU_ASSERT_TRUE(list_size_seglib() == 1);

    free(data1); free(data2); free(data3); free(data4); free(data5); free(data6);
}

/// paginacion

void test_print_framo() {
    for (int i=0; i<10; i++)
        ocupar_frame_framo(i, cfg->TAMANIO_PAGINA, 14);
    for (int i=20; i<30; i++)
        ocupar_frame_framo(i, cfg->TAMANIO_PAGINA/2, 27);

    liberar_frame_framo(3);
    liberar_frame_framo(6);

    print_framo(false);
}

void test_primer_frame_libre() {
    uint32_t inicio;
    // 00000000 00000000 00000000 00000000
    print_framo(false);
    uint32_t cero = 0;
    CU_ASSERT_TRUE(primer_frame_libre_framo(2, &inicio) == cero && inicio == 0);

    for (int i=0; i<10; i++)
        ocupar_frame_framo(i, cfg->TAMANIO_PAGINA, 6);
    for (int i=10; i<20; i++)
        ocupar_frame_framo(i, cfg->TAMANIO_PAGINA, 9);
    ocupar_frame_framo(20, 15, 9);

    // 66666666 66999999 99998000 00000000

    liberar_frame_framo(3);
    liberar_frame_framo(6);

    // 66606606 66999999 99998000 00000000

    print_framo(false);
    uint32_t tres = 3;
    CU_ASSERT_TRUE(primer_frame_libre_framo(2, &inicio) == tres && inicio == 0);
    CU_ASSERT_TRUE(primer_frame_libre_framo(9, &inicio) == 20 && inicio == 15);

    for (int i=21; i<(cfg->TAMANIO_MEMORIA/cfg->TAMANIO_PAGINA); i++)
        ocupar_frame_framo(i, cfg->TAMANIO_PAGINA, 2);
    ocupar_frame_framo(3, cfg->TAMANIO_PAGINA, 2);
    ocupar_frame_framo(6, cfg->TAMANIO_PAGINA, 2);

    print_framo(false);
    CU_ASSERT_TRUE(primer_frame_libre_framo(11, &inicio) == -1);
}

void test_ocupar_frames() {
    uint32_t patota1 = 7, patota2 = 3, patota3 = 2;

    size_t size_of_pan = cfg->TAMANIO_PAGINA + 27;
    void* pan = malloc(size_of_pan);
    memset(pan, 0xCA, size_of_pan);

    CU_ASSERT_TRUE(append_data_to_patota_en_mp(pan, size_of_pan, patota1));
    mem_hexdump(memoria_principal, 400);
    print_framo(false);

    // 76000000 00000000 00000000 00000000

    size_t size_of_salmon = cfg->TAMANIO_PAGINA*3 - 10;
    void* salmon = malloc(size_of_salmon);
    memset(salmon, 0xFA, size_of_salmon);

    CU_ASSERT_TRUE(append_data_to_patota_en_mp(salmon, size_of_salmon, patota2));
    mem_hexdump(memoria_principal, 400);
    print_framo(false);

    // 76332000 00000000 00000000 00000000

    size_t size_of_salame = cfg->TAMANIO_PAGINA*2;
    void* salame = malloc(size_of_salame);
    memset(salame, 0xFE, size_of_salame);

    CU_ASSERT_TRUE(append_data_to_patota_en_mp(salame, size_of_salame, patota1));
    mem_hexdump(memoria_principal, 400);
    print_framo(false);

    // 77332760 00000000 00000000 00000000

    size_t size_of_queso = cfg->TAMANIO_PAGINA - 27;
    void* queso = malloc(size_of_queso);
    memset(queso, 0xFE, size_of_queso);

    CU_ASSERT_TRUE(append_data_to_patota_en_mp(queso, size_of_queso, patota1));
    mem_hexdump(memoria_principal, 400);
    print_framo(false);

    // 77332770 00000000 00000000 00000000

    size_t size_of_chocolate = 14;
    void* chocolate = malloc(size_of_chocolate);
    memset(chocolate, 0xC0, size_of_chocolate);

    CU_ASSERT_TRUE(append_data_to_patota_en_mp(chocolate, size_of_chocolate, patota3));
    mem_hexdump(memoria_principal, 1050);
    print_framo(false);

    // 77332771 00000000 00000000 00000000

    free(pan); free(queso); free(salmon); free(chocolate); free(salame); // comida gratis
}

CU_TestInfo tests_memoria[] = {
    // { "Test print seglib/segus", test_print },
    // { "Test proximo hueco first fit", test_hueco_first_fit },
    // { "Test proximo hueco first fit (no hay)", test_hueco_first_fit_no_hay },
    // { "Test proximo hueco best fit (1)", test_hueco_best_fit1 },
    // { "Test proximo hueco best fit (2)", test_hueco_best_fit2 },
    // { "Test proximo hueco best fit (no hay)", test_hueco_best_fit_no_hay },
    // { "Test meter nuevo segmento (best fit - justo)", test_meter_segmento_ocupa_hueco_entero_bf },
    // { "Test meter nuevo segmento (first fit - justo)", test_meter_segmento_ocupa_hueco_entero_ff },
    // { "Test meter nuevo segmento (best fit)", test_meter_segmento_bf },
    // { "Test meter nuevo segmento (first fit)", test_meter_segmento_ff },
    // { "Test meter segmento en MP (first fit)", test_meter_segmento_en_mp_ff },
    // { "Test meter segmento en MP (best fit)", test_meter_segmento_en_mp_bf },
    // { "Test COMPACTACION", test_compactacion },
    { "Test print framo", test_print_framo },
    { "Test primer frame libre", test_primer_frame_libre },
    { "Test ocupar frames", test_ocupar_frames },
    CU_TEST_INFO_NULL,
};
