#include "../include/estructuras.h"

static uint32_t tamanio_static = 0;

static bool seg_entra_en_hueco(void* segmento) {
    segmento_t* seg = (segmento_t*) segmento;
    return seg->tamanio >= tamanio_static;
}

static void* hueco_menor(void* h1, void* h2) {
    segmento_t* hueco1 = (segmento_t*) h1;
    segmento_t* hueco2 = (segmento_t*) h2;
    return hueco1->tamanio < hueco2->tamanio? h1 : h2;
}

segmento_t* proximo_hueco_best_fit(uint32_t tamanio) {
    tamanio_static = tamanio;
    t_list* huecos_disponibles = list_filter(segmentos_libres, &seg_entra_en_hueco);
    if (list_size(huecos_disponibles) == 0) {
        list_destroy(huecos_disponibles);
        return NULL;
    }
    segmento_t* hueco_menor = (segmento_t*) list_get_minimum(huecos_disponibles, (void*) &hueco_menor);

    return hueco_menor;
}

segmento_t* proximo_hueco_first_fit(uint32_t tamanio) {
    tamanio_static = tamanio;
    segmento_t* hueco_disponible = (segmento_t*) list_find(segmentos_libres, &seg_entra_en_hueco);

    return hueco_disponible;
}

;;;