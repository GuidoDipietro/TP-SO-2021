#include "../include/manejo_memoria.h"

extern t_log* logger;
extern t_list* segmentos_libres;
extern void* memoria_principal;
extern uint32_t memoria_disponible;
extern segmento_t* (*proximo_hueco)(uint32_t);

////// MANEJO MEMORIA PRINCIPAL - SEGMENTACION

bool entra_en_mp(uint32_t tamanio) {
    return memoria_disponible >= tamanio;
}

// Meter chorizo de bytes en MP y actualiza listas de registro.
// Si se llama a esta func es porque ya se sabe que entra
bool meter_segmento_en_mp(void* data, uint32_t size) {
    segmento_t* hueco_victima = (*proximo_hueco)(size);
    if (hueco_victima == NULL)
        return false; // no hay hueco (no deberia pasar)

    uint32_t inicio = hueco_victima->inicio;
    memcpy_segmento_en_mp(hueco_victima->inicio, data, size);

    if (!meter_segmento_actualizar_hueco(hueco_victima, size)) {
        log_error(logger, "Error catastrofico metiendo segmento en MP");
        return false;
    }

    memoria_disponible -= size;
    if (memoria_disponible < 0) {
        // Algo dentro de mi me dijo: pone esto por las dudas
        log_error(logger, "ROMPISTE TODO, QUE HICISTE?");
        return false;
    }

    list_add_segus(new_segmento(0, inicio, size)); // Al final, por si exploto todo antes

    return true;
}

bool realloc_segmento_en_mp(uint32_t inicio_origen, uint32_t inicio_destino) {
    segmento_t* segmento = list_find_by_inicio_segus(inicio_origen);
    segmento_t* hueco = list_find_first_by_inicio_seglib(inicio_destino);

    // Si ponen cualquiera
    if (segmento == NULL) return false;

    // Si hueco es NULL, significa que se mueve menos que su tamanio (raro, pero bueno!)
    if (hueco == NULL) {
        hueco = list_find_first_by_inicio_seglib(inicio_origen+segmento->tamanio);
        print_segmento_t(hueco);
        if (hueco == NULL) return false; // esta trabado, no se puede mover
        if (hueco->tamanio < inicio_destino - inicio_origen) return false; // no entra en el hueco

        uint32_t delta = inicio_destino - inicio_origen;

        segmento_t* posible_hueco_anterior = list_find_first_by_inicio_seglib(inicio_origen-1);
        if (posible_hueco_anterior)
            posible_hueco_anterior->tamanio += delta;
        else {
            segmento_t* nuevo_hueco_generado = new_segmento(0, inicio_origen, delta);
            list_add_seglib(nuevo_hueco_generado);
        }

        // Movemos el segmento en MP, dejando 0s en el lugar anterior porque me gusta
        void* data = malloc(segmento->tamanio);
        memcpy(data, memoria_principal+segmento->inicio, segmento->tamanio);
        memset(memoria_principal+segmento->inicio, 0, segmento->tamanio);
        memcpy(memoria_principal+inicio_destino, data, segmento->tamanio);
        free(data);

        segmento->inicio = inicio_destino;
        hueco->inicio = inicio_destino+segmento->tamanio;
        hueco->tamanio -= delta;
        remove_zero_sized_gap_seglib();
        return true;
    }

    // Si no entra en el hueco
    if (hueco->tamanio < segmento->tamanio) return false;

    // AGREGAR SOPORTE PARA SHIFTS HACIA ARRIBA (Y REPENSAR TODA ESTA MIERDA)

    /// Ahora... Si nada de eso sucedio... Agarrate:

    // Cuando un amigo se va... </3
    segmento_t* nuevo_hueco_generado = new_segmento(0, segmento->inicio, inicio_destino-inicio_origen);

    // Movemos el segmento en MP, dejando 0s en el lugar anterior porque me gusta
    void* data = malloc(segmento->tamanio);
    memcpy(data, memoria_principal+segmento->inicio, segmento->tamanio);
    memset(memoria_principal+segmento->inicio, 0, segmento->tamanio);
    memcpy(memoria_principal+inicio_destino, data, segmento->tamanio);
    free(data);

    // Actualizamos las listas de registro
    segmento->inicio = inicio_destino;                                      // trivial
    hueco->tamanio -= (segmento->tamanio + inicio_destino-hueco->inicio);   // tamanio - segmento - hueco mitosis
    hueco->inicio = inicio_destino + segmento->tamanio;                     // trivial

    // Si hay que mergear con otro/s hueco/s... hele aqui
    segmento_t* posible_hueco_anterior = list_find_first_by_inicio_seglib(
        nuevo_hueco_generado->inicio-1
    );
    segmento_t* posible_hueco_posterior = list_find_first_by_inicio_seglib(
        nuevo_hueco_generado->inicio+nuevo_hueco_generado->tamanio
    );
    if (posible_hueco_anterior != NULL) {
        nuevo_hueco_generado->inicio = posible_hueco_anterior->inicio;
        nuevo_hueco_generado->tamanio += posible_hueco_anterior->tamanio;
        posible_hueco_anterior->tamanio = 0;
    }
    if (posible_hueco_posterior != NULL) {
        nuevo_hueco_generado->tamanio += posible_hueco_posterior->tamanio;
        posible_hueco_posterior->tamanio = 0;
    }

    list_add_seglib(nuevo_hueco_generado);

    remove_zero_sized_gap_seglib();
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