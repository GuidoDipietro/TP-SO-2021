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

// abrochense el cinturon
bool mover_segmento_en_mp(uint32_t inicio_origen, uint32_t inicio_destino) {
    int64_t despl = inicio_destino - inicio_origen;
    if (despl == 0) return true; // sin efecto

    segmento_t* segmento = list_find_by_inicio_segus(inicio_origen);
    if (segmento == NULL) return false; // no existe segmento con ese inicio

    segmento_t* hueco = list_find_first_by_inicio_seglib(inicio_destino);
    // se mueve al segmento dentro de un hueco existente
    if (hueco != NULL) {
        // no entra en el hueco...
        if ((hueco->inicio+hueco->tamanio - inicio_destino) < segmento->tamanio) {
            // se mueve para ATRAS a un hueco adyacente, un poquitin, podria entrar
            if (despl<0 && (-1*despl < segmento->tamanio)) {
                segmento_t* nuevo_hueco = new_segmento(0, inicio_destino+segmento->tamanio, -1*despl);
                list_add_seglib(nuevo_hueco);
                segmento_t* hueco_adyacente = list_find_first_by_inicio_seglib(inicio_destino);
                hueco_adyacente->tamanio -= -1*despl;
                if (hueco_adyacente->tamanio == 0) remove_zero_sized_gap_seglib();
            }
            else return false; // nop, definitivamente no entraba
        }
        // entra en hueco
        else {
            // Hueco despues del segmento
            segmento_t* hueco_mitosis = new_segmento(
                0, inicio_destino+segmento->tamanio, // inicia en el fin del segmento realocado
                hueco->inicio+hueco->tamanio - (inicio_destino+segmento->tamanio) // tamanio: fin - inicio
            );
            if (hueco_mitosis->tamanio != 0) list_add_seglib(hueco_mitosis);
            else free(hueco_mitosis);

            // Hueco antes del segmento
            hueco->tamanio = inicio_destino - hueco->inicio;

            // Hueco que deja el segmento al irse del lugar anterior (2 posibles casos de merge)
            segmento_t* hueco_anterior = list_find_first_by_inicio_seglib(inicio_origen-1);
                // merge anterior
            if (hueco_anterior != NULL && inicio_origen == (hueco_anterior->inicio+hueco_anterior->tamanio)) {
                hueco_anterior->tamanio += segmento->tamanio;
            }
                // merge posterior
            if (inicio_origen+segmento->tamanio == hueco->inicio) {
                    // posible merge con hueco_anterior, tambien
                if (hueco_anterior != NULL && (hueco_anterior->inicio+hueco_anterior->tamanio)==hueco->inicio) {
                    hueco_anterior->tamanio += hueco->tamanio;
                    hueco->tamanio = 0;
                    remove_zero_sized_gap_seglib();
                }
                else {
                    hueco->inicio = inicio_origen;
                    hueco->tamanio += segmento->tamanio;
                }
            }
                // sin merge
            else {
                segmento_t* hueco_generado = new_segmento(0, inicio_origen, segmento->tamanio);
                list_add_seglib(hueco_generado);
            }
        }
    }
    else {
        // se mueve menos que lo que ocupa (inicio cae dentro suyo)
        if (despl>0 && (despl < segmento->tamanio)) {
            segmento_t* hueco_posterior = list_find_first_by_inicio_seglib(inicio_origen+segmento->tamanio);
            if (hueco_posterior == NULL) return false; // estaba trabado, no se puede
            if (hueco_posterior->inicio + hueco_posterior->tamanio < inicio_destino+segmento->tamanio)
                return false; // pisa otro segmento
            hueco_posterior->inicio += despl;
            hueco_posterior->tamanio -= despl;
            if (hueco_posterior->tamanio == 0) remove_zero_sized_gap_seglib();

            segmento_t* hueco_anterior = list_find_first_by_inicio_seglib(inicio_origen-1);
            if (hueco_anterior == NULL) { // estaba pegadito a otro segmento
                hueco_anterior = new_segmento(0, inicio_origen, despl);
                list_add_seglib(hueco_anterior);
            }
            else hueco_anterior->tamanio += despl;
        }
        else return false; // pisa otro segmento
    }

    // Si llegamos hasta aca, podemos hacer el realloc fisico
    segmento->inicio = inicio_destino;
    realloc_segmento_en_mp(inicio_origen, inicio_destino, segmento->tamanio);
    return true; // hooray
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