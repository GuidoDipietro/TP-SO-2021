#include "../include/test_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;

extern t_list* segmentos_libres;
extern t_list* segmentos_usados;
extern t_list* ts_patotas;
extern t_list* ts_tripulantes;
extern segmento_t* (*proximo_hueco)(uint32_t);

extern t_list* tp_patotas;
extern char* puntero_a_bits;
extern t_bitarray* bitarray_frames;

extern uint32_t memoria_disponible;
extern void* memoria_principal;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

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

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10) != INICIO_INVALIDO);

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

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 50) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2, 10) != INICIO_INVALIDO);

    puts("Dump primeros 65 bytes MP DESPUES:");
    mem_hexdump(memoria_principal, 65);

    print_seglib();
    __asm__ __volatile__ ("call print_segus");

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

    CU_ASSERT_TRUE(meter_segmento_en_mp(data1, 10) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data2,  5) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data3, 20) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data4,  7) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data5, 16) != INICIO_INVALIDO);
    CU_ASSERT_TRUE(meter_segmento_en_mp(data6, 30) != INICIO_INVALIDO);

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

    print_seglib();
    print_segus();
    mem_hexdump(memoria_principal, 100);

    CU_ASSERT_TRUE(list_size_seglib() == 1);

    free(data1); free(data2); free(data3); free(data4); free(data5); free(data6);
}

void test_stringify() {
    segmento_t* test = new_segmento(2, 51914, 27);
    char* str = stringify_segmento_t(test);

    print_segmento_t(test);
    printf("\n<<%s>>\n", str);

    CU_ASSERT_TRUE(strcmp("SEGMENTO:   2 | INICIO: 0x0000caca | TAM:    27b", str) == 0);

    free(str);
    free(test);

    ts_patota_t* tabla = malloc(sizeof(ts_patota_t));
    segmento_t* seg_pcb = new_segmento(3, 69, 44);
    segmento_t* seg_tareas = new_segmento(0, 777, 15);
    tabla->pcb = seg_pcb;
    tabla->tareas = seg_tareas;
    tabla->pid = 8;

    char* str2 = stringify_ts_patota_t(tabla, "14/27/2049 06:09:69 A.M.");
    printf("\n<<%s>>\n", str2);

    CU_ASSERT_TRUE(strcmp("------------------------------\n\
Dump: 14/27/2049 06:09:69 A.M.\n\
PROCESO:   8 | SEGMENTO:   3 | INICIO: 0x00000045 | TAM:    44b\n\
PROCESO:   8 | SEGMENTO:   0 | INICIO: 0x00000309 | TAM:    15b\n\
-------------------------", str2) == 0);

    free(str2);
    free_ts_patota_t(tabla);
}

void test_print_bitarray() {
    for (int i=0; i<10; i++)
        ocupar_frame_frambit(i);
    for (int i=20; i<30; i++)
        ocupar_frame_frambit(i);

    liberar_frame_frambit(3);
    liberar_frame_frambit(6);

    print_bitarray_frames();
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
    { "Test meter segmento en MP (first fit)", test_meter_segmento_en_mp_ff },
    { "Test meter segmento en MP (best fit)", test_meter_segmento_en_mp_bf },
    { "Test COMPACTACION", test_compactacion },
    { "Test stringify", test_stringify },
    { "Test print bitarray", test_print_bitarray },
    CU_TEST_INFO_NULL,
};