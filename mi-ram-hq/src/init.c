#include "../include/init.h"

uint8_t cargar_configuracion(t_config_mrhq* config, t_log* log) {
    t_config* cfg = config_create("mi-ram-hq.config");

    if(cfg == NULL) {
        log_error(log, "No se encontro mi-ram-hq.config");
        return 0;
    }

    char* properties[] = {
            "TAMANIO_MEMORIA",
            "ESQUEMA_MEMORIA",
            "TAMANIO_PAGINA",
            "TAMANIO_SWAP",
            "PATH_SWAP",
            "ALGORITMO_REEMPLAZO",
            "PUERTO",
            NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg, properties)) {
        log_error(log, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg);
        return 0;
    }

    config->TAMANIO_MEMORIA = config_get_int_value(cfg, "TAMANIO_MEMORIA");
    config->ESQUEMA_MEMORIA = strdup(config_get_string_value(cfg, "ESQUEMA_MEMORIA"));
    config->TAMANIO_PAGINA = config_get_int_value(cfg, "TAMANIO_PAGINA");
    config->TAMANIO_SWAP = config_get_int_value(cfg, "TAMANIO_SWAP");
    config->PATH_SWAP = strdup(config_get_string_value(cfg, "PATH_SWAP"));
    config->ALGORITMO_REEMPLAZO = strdup(config_get_string_value(cfg, "ALGORITMO_REEMPLAZO"));
    config->PUERTO = config_get_int_value(cfg, "PUERTO");

    log_info(log, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg);

    return 1;
}

void cerrar_programa(t_config_mrhq* cfg, t_log* log) {
    log_destroy(log);

    if(cfg->ALGORITMO_REEMPLAZO != NULL)
        free(cfg->ALGORITMO_REEMPLAZO);

    if(cfg->PATH_SWAP != NULL)
        free(cfg->PATH_SWAP);

    if(cfg->ESQUEMA_MEMORIA != NULL)
        free(cfg->ESQUEMA_MEMORIA);

    free(cfg);
}