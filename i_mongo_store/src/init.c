#include "../include/init.h"

uint8_t cargar_configuracion(t_config_ims* config, t_log* log) {
    t_config* cfg = config_create("i_mongo_store.config");

    if(cfg == NULL) {
        log_error(log, "No se encontro i_mongo_store.config");
        return 0;
    }

    char* properties[] = {
            "PUNTO_MONTAJE",
            "PUERTO",
            "TIEMPO_SINCRONIZACION",
            "POSICIONES_SABOTAJE",
            NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg, properties)) {
        log_error(log, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg);
        return 0;
    }

    config->PUNTO_MONTAJE = strdup(config_get_string_value(cfg, "PUNTO_MONTAJE"));
    config->PUERTO = config_get_int_value(cfg, "PUERTO");
    config->TIEMPO_SINCRONIZACION = config_get_long_value(cfg, "TIEMPO_SINCRONIZACION");

    char** posiciones_sabotaje = config_get_array_value(cfg, "POSICIONES_SABOTAJE");
    config->POSICIONES_SABOTAJE = extraer_posiciones(posiciones_sabotaje);
    config_free_array_value(&posiciones_sabotaje);

    log_info(log, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg);

    return 1;
}

void cerrar_programa(t_log* log, t_config_ims* cfg) {
    log_destroy(log);

    if(cfg->PUNTO_MONTAJE != NULL)
        free(cfg->PUNTO_MONTAJE);

    if(cfg->POSICIONES_SABOTAJE != NULL)
        list_destroy_and_destroy_elements(cfg->POSICIONES_SABOTAJE, free_t_posicion);

    free(cfg);
}