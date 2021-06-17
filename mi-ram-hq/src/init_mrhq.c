#include "../include/init_mrhq.h"

// varios
t_log* logger;
t_config_mrhq* cfg;

// segmentacion
t_list* segmentos_libres;
t_list* segmentos_usados;
uint32_t memoria_disponible;
t_list* ts_patotas;
t_list* ts_tripulantes;
segmento_t* (*proximo_hueco)(uint32_t);

// paginacion
char* puntero_a_bits;
t_bitarray* bitarray_paginas;
t_list* tp_patotas;

void* memoria_principal;

////// funciones

uint8_t init() {
    cfg = initialize_cfg();
    logger = log_create("mi-ram-hq.log", MODULENAME, false, LOG_LEVEL_INFO);
    iniciar_mutex();

    return 1;
}

uint8_t cargar_configuracion(char* path) {
    t_config* cfg_file = config_create(path);

    if(cfg_file == NULL) {
        log_error(logger, "No se encontro mi-ram-hq.config");
        return 0;
    }

    char* properties[] = {
        "TAMANIO_MEMORIA",
        "ESQUEMA_MEMORIA",
        "TAMANIO_PAGINA",
        "TAMANIO_SWAP",
        "PATH_SWAP",
        "ALGORITMO_REEMPLAZO",
        "CRITERIO_SELECCION",
        "PUERTO",
        "IP",
        NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg_file, properties)) {
        log_error(logger, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg_file);
        return 0;
    }

    cfg->TAMANIO_MEMORIA = config_get_int_value(cfg_file, "TAMANIO_MEMORIA");
    cfg->ESQUEMA_MEMORIA = strdup(config_get_string_value(cfg_file, "ESQUEMA_MEMORIA"));
    cfg->TAMANIO_PAGINA = config_get_int_value(cfg_file, "TAMANIO_PAGINA");
    cfg->TAMANIO_SWAP = config_get_int_value(cfg_file, "TAMANIO_SWAP");
    cfg->PATH_SWAP = strdup(config_get_string_value(cfg_file, "PATH_SWAP"));
    cfg->ALGORITMO_REEMPLAZO = strdup(config_get_string_value(cfg_file, "ALGORITMO_REEMPLAZO"));
    cfg->CRITERIO_SELECCION = strdup(config_get_string_value(cfg_file, "CRITERIO_SELECCION"));

    proximo_hueco = strcmp(cfg->CRITERIO_SELECCION, "FF") == 0
                    ? &proximo_hueco_first_fit
                    : &proximo_hueco_best_fit;

    cfg->PUERTO = config_get_int_value(cfg_file, "PUERTO");
    cfg->IP = config_get_int_value(cfg_file, "IP");

    log_info(logger, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg_file);

    return 1;
}

uint8_t cargar_memoria() {
    memoria_principal = malloc(cfg->TAMANIO_MEMORIA); // void*
    if (memoria_principal == NULL) {
        log_error(logger, "Fallo en el malloc a memoria_principal");
        return 0;
    }
    memset(memoria_principal, 0, cfg->TAMANIO_MEMORIA);
    memoria_disponible = cfg->TAMANIO_MEMORIA; // int

    // Segmentacion
    if (strcmp(cfg->ESQUEMA_MEMORIA,"SEGMENTACION")==0) {
        segmentos_libres = list_create();
        segmento_t* hueco = new_segmento(0, 0, cfg->TAMANIO_MEMORIA);
        if (hueco == NULL) {
            log_error(logger, "Fallo en la creacion de t_list* segmentos_libres");
            asesinar_seglib();
            return 0;
        }
        list_add(segmentos_libres, (void*) hueco);

        segmentos_usados = list_create();
        ts_patotas = list_create();
        ts_tripulantes = list_create();
    }
    // Paginacion
    else {
        tp_patotas = list_create();
        uint32_t cant_paginas = cfg->TAMANIO_MEMORIA / cfg->TAMANIO_PAGINA;
        puntero_a_bits = malloc(cant_paginas);
        memset(puntero_a_bits, 0, cant_paginas);
        bitarray_paginas = bitarray_create_with_mode(puntero_a_bits, cant_paginas, LSB_FIRST);
    }

    return 1;
}

void cerrar_programa() {
    bool segmentacion = strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION")==0;
    log_destroy(logger);

    free(cfg->ALGORITMO_REEMPLAZO);
    free(cfg->PATH_SWAP);
    free(cfg->ESQUEMA_MEMORIA);
    free(cfg->CRITERIO_SELECCION);
    free(cfg);

    // masacre
    if (segmentacion) {
        asesinar_seglib();
        asesinar_segus();
        asesinar_tspatotas();
        asesinar_tstripulantes();
    }
    else {
        asesinar_tppatotas();
        bitarray_destroy(bitarray_paginas);
        free(puntero_a_bits);
    }

    free(memoria_principal);
    finalizar_mutex();
}
