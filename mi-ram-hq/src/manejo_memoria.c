#include "../include/manejo_memoria.h"

static void* hueco_menor(void* h1, void* h2) {
    segmento_t* hueco1 = (segmento_t*) h1;
    segmento_t* hueco2 = (segmento_t*) h2;
    return hueco1->tamanio < hueco2->tamanio? h1 : h2;
}
segmento_t* proximo_hueco_best_fit(uint32_t tamanio) {
    t_list* huecos_disponibles = list_filter_by_min_size_seglib(tamanio);
    if (list_size(huecos_disponibles) == 0) {
        list_destroy(huecos_disponibles);
        return NULL;
    }
    return (segmento_t*) list_get_minimum(huecos_disponibles, (void*) &hueco_menor);
}

segmento_t* proximo_hueco_first_fit(uint32_t tamanio) {
	return list_find_first_by_min_size_seglib(tamanio);
}

void compactar_segmentos_libres() {
    segmento_t* suma_de_huecos = list_add_all_holes_seglib();
    list_clean_seglib();
    list_add_seglib(suma_de_huecos);
}