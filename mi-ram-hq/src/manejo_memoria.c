#include "../include/manejo_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;
extern t_list* segmentos_libres;
extern void* memoria_principal;
extern uint32_t memoria_disponible;
extern segmento_t* (*proximo_hueco)(uint32_t);

extern t_list* tp_patotas;
extern char* puntero_a_bits;
extern t_bitarray* bitarray_frames;

extern pthread_mutex_t MUTEX_MP;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

////// MANEJO MEMORIA PRINCIPAL - SEGMENTACION

bool entra_en_mp(uint32_t tamanio) {
    return memoria_disponible >= tamanio;
}

// Meter chorizo de bytes en MP y actualiza listas de registro.
// Si se llama a esta func es porque ya se sabe que entra
// Devuelve la Dir fisica del segmento, o INICIO_INVALIDO si explota (no deberia pasar)
uint32_t meter_segmento_en_mp(void* data, uint32_t size) {
    segmento_t* hueco_victima = (*proximo_hueco)(size);
    if (hueco_victima == NULL) {
        compactar_mp();
        segmento_t* hueco_victima = (*proximo_hueco)(size);
        if (hueco_victima == NULL)
            return INICIO_INVALIDO; // no hay hueco (no deberia pasar)
    }

    uint32_t inicio = hueco_victima->inicio;
    memcpy_segmento_en_mp(hueco_victima->inicio, data, size);

    if (!meter_segmento_actualizar_hueco(hueco_victima, size)) {
        log_error(logger, "Error catastrofico metiendo segmento en MP");
        return INICIO_INVALIDO;
    }

    memoria_disponible -= size;
    if (memoria_disponible < 0) {
        // Algo dentro de mi me dijo: pone esto por las dudas
        log_error(logger, "ROMPISTE TODO, QUE HICISTE?");
        return INICIO_INVALIDO;
    }

    list_add_segus(new_segmento(0, inicio, size)); // Al final, por si exploto todo antes

    return inicio;
}

bool eliminar_segmento_de_mp(uint32_t inicio) {
    segmento_t* segmento = list_find_by_inicio_segus(inicio);
    if (segmento == NULL) return false;

    // Nuevo hueco
    segmento_t* hueco_generado = new_segmento(0, segmento->inicio, segmento->tamanio);
    segmento_t* hueco_posterior = list_find_first_by_inicio_seglib(hueco_generado->inicio+hueco_generado->tamanio);
    segmento_t* hueco_anterior = list_find_first_by_inicio_seglib(hueco_generado->inicio-1);
    if (hueco_posterior) {
        hueco_generado->tamanio += hueco_posterior->tamanio;
        hueco_posterior->tamanio = 0;
        remove_zero_sized_gap_seglib();
    }
    if (hueco_anterior) {
        hueco_generado->inicio = hueco_anterior->inicio;
        hueco_generado->tamanio += hueco_anterior->tamanio;
        hueco_anterior->tamanio = 0;
        remove_zero_sized_gap_seglib();
    }
    list_add_seglib(hueco_generado);

    // Memset fisico
    memset_0_segmento_en_mp(segmento->inicio, segmento->tamanio);
    list_remove_by_inicio_segus(segmento->inicio);
    return true;
}

static bool compactar_mp_iteracion(uint32_t i) {
    segmento_t* segmento = list_get_segus(i);
    if (segmento == NULL) return false;     // no existe!!
    if (segmento->inicio == 0) return true; // es el primero y esta pegadito al techo

    segmento_t* hueco = list_find_first_by_inicio_seglib(segmento->inicio-1);
    if (hueco == NULL) return true;         // ya esta compactado

    uint32_t inicio_original = segmento->inicio;
    uint32_t inicio_destino = hueco->inicio;

    // Muevo segmento y muevo hueco
    segmento->inicio = hueco->inicio;
    list_sort_segus();

    hueco->inicio = segmento->inicio + segmento->tamanio;

    // Posible merge
    segmento_t* hueco_posterior = list_find_first_by_inicio_seglib(hueco->inicio+hueco->tamanio);
    if (hueco_posterior) {
        hueco->tamanio += hueco_posterior->tamanio;
        hueco_posterior->tamanio = 0;
        remove_zero_sized_gap_seglib();
    }

    // Movemos el segmento en la memoria fisica
    realloc_segmento_en_mp(inicio_original, inicio_destino, segmento->tamanio);
    return true;
}
bool compactar_mp() {
    if (list_is_empty_segus()) return true;

    uint32_t segmentos = list_size_segus();
    for (int i=0; i<segmentos+1; i++)
        if(!compactar_mp_iteracion(i)) return false;
    return true;
}

////// UTILS SEGMENTOS_LIBRES

static void* hueco_menor(void* h1, void* h2) {
    segmento_t* hueco1 = (segmento_t*) h1;
    segmento_t* hueco2 = (segmento_t*) h2;
    return hueco1->tamanio < hueco2->tamanio? h1 : h2;
}

// Meto un segmento nuevo a la MP, tengo menos huecos
bool meter_segmento_actualizar_hueco(segmento_t* hueco_target, uint32_t tamanio) {
    // Cosas que no deberian pasar nunca:
    if (hueco_target == NULL || (hueco_target->tamanio < tamanio)) //??
        return false;

    if (hueco_target->tamanio == tamanio) {
        hueco_target->tamanio = 0;
        remove_zero_sized_gap_seglib(); // no es lo mas eficiente/elegante, pero si lo mas facil
        return true;
    }

    hueco_target->inicio += tamanio;
    hueco_target->tamanio -= tamanio;
    return true;
}

// De todos los huecos, elige el que minimiza el espacio desaprovechado (si hay empate, el ultimo)
segmento_t* proximo_hueco_best_fit(uint32_t tamanio) {
    t_list* huecos_disponibles = list_filter_by_min_size_seglib(tamanio);

    if (list_size(huecos_disponibles) == 0) {
        list_destroy(huecos_disponibles);
        return NULL;
    }
    segmento_t* ret = (segmento_t*) list_get_minimum(huecos_disponibles, (void*) &hueco_menor);
    list_destroy(huecos_disponibles);

    return ret;
}

// De todos los huecos, elige el primero en el que entra el tamanio dado
segmento_t* proximo_hueco_first_fit(uint32_t tamanio) {
    return list_find_first_by_min_size_seglib(tamanio);
}

void compactar_segmentos_libres() {
    list_clean_seglib();
    list_add_seglib(
        new_segmento(
            0,
            cfg->TAMANIO_MEMORIA-memoria_disponible,
            memoria_disponible
        )
    );
}


////// MANEJO MEMORIA PRINCIPAL - PAGINACION

static bool meter_pagina_en_mp(void* data) {
    int64_t frame_libre = primer_frame_libre_frambit();
    if (frame_libre == -1) return false; // posteriormente: implementar memoria virtual

    uint32_t nro_frame = frame_libre; // una especie de casteo porlas

    ocupar_frame_frambit(nro_frame);
    memcpy_pagina_en_frame_mp(nro_frame, data);
    return true;
}

bool meter_choclo_paginado_en_mp(void* data, size_t size) {
    uint32_t t_pag = cfg->TAMANIO_PAGINA;
    uint32_t cant_paginas = size%t_pag==0? size/t_pag : size/t_pag + 1;

    void* padded_data = malloc(cant_paginas * t_pag);
    memset(padded_data, 0, cant_paginas * t_pag);
    memcpy(padded_data, data, size);

    void* buf = malloc(t_pag);
    for (uint32_t i=0; i<cant_paginas; i++) {
        memcpy(buf, padded_data+i*t_pag, t_pag);
        if (!meter_pagina_en_mp(buf)) {
            free(buf);
            return false;
        }
    }

    free(padded_data);
    free(buf);
    return true;
}