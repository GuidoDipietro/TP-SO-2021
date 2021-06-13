#include "../include/init_mrhq.h"

t_log* logger;
t_config_mrhq* cfg;
t_list* segmentos_libres;

uint8_t init() {
    cfg = initialize_cfg();
    logger = log_create("mi-ram-hq.log", MODULENAME, false, LOG_LEVEL_INFO);
    iniciar_mutex();

    return 1;
}

uint8_t cargar_configuracion() {
    t_config* cfg_file = config_create("mi-ram-hq.config");

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
    cfg->PUERTO = config_get_int_value(cfg_file, "PUERTO");
    cfg->IP = config_get_int_value(cfg_file, "IP");

    log_info(logger, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg_file);

    return 1;
}

uint8_t cargar_memoria() {
    segmentos_libres = list_create();

    segmento_t* hueco = malloc(sizeof(segmento_t));
    if (hueco == NULL) {
        log_error(logger, "Fallo en la creacion de t_list* segmentos_libres");
        return 0;
    }
    hueco->inicio = 0;
    hueco->tamanio = cfg->TAMANIO_MEMORIA;

    list_add(segmentos_libres, (void*) hueco);

    return 1;
}

void cerrar_programa() {
    log_destroy(logger);

    free(cfg->ALGORITMO_REEMPLAZO);
    free(cfg->PATH_SWAP);
    free(cfg->ESQUEMA_MEMORIA);
    free(cfg);

    asesinar_seglib();
    finalizar_mutex();
}