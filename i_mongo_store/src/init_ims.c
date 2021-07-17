#include "../include/init_ims.h"

t_config_ims* cfg;

pthread_t HILO_SINCRONIZADOR;

void iniciar_semaforos() {
    pthread_mutex_init(&MUTEX_BITARRAY, NULL);
    pthread_mutex_init(&MUTEX_BLOCKS, NULL);
    pthread_mutex_init(&MUTEX_LISTA_ARCHIVOS, NULL);
    pthread_mutex_init(&MUTEX_COLA_OPERACIONES, NULL);
    sem_init(&OPERACIONES_PENDIENTES, 0, 0);
    sem_init(&DISCO_LIBRE, 0, 0);
    sem_init(&sem_sabotaje, 0, 0);
    sem_init(&sem_inicio_fsck, 0, 0);
}

void iniciar_sincronizador() {
    pthread_create(&HILO_SINCRONIZADOR, NULL, (void*) sincronizador, NULL);
    pthread_detach(HILO_SINCRONIZADOR);
}

bool existe_directorio(char* path) {
    DIR* dir = opendir(path);
    if(dir) {
        closedir(dir);
        return true;
    } else {
        log_error(logger, "No existe el directorio %s", path);
        return false;
    }
}

uint8_t cargar_configuracion() {
    iniciar_semaforos();

    t_config* cfg_file = config_create("i_mongo_store.config");

    if(cfg_file == NULL) {
        log_error(logger, "No se encontro i_mongo_store.config");
        return 0;
    }

    char* properties[] = {
        "PUNTO_MONTAJE",
        "PUERTO",
        "TIEMPO_SINCRONIZACION",
        "POSICIONES_SABOTAJE",
        "BLOCKS",
        "BLOCK_SIZE",
        NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg_file, properties)) {
        log_error(logger, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg_file);
        return 0;
    }

    cfg->PUNTO_MONTAJE = strdup(config_get_string_value(cfg_file, "PUNTO_MONTAJE"));

    char* p1 = path_bitacora();
    char* p2 = path_files();
    if(!existe_directorio(cfg->PUNTO_MONTAJE) || !existe_directorio(p2) || !existe_directorio(p1))
        return 0;

    cfg->PUERTO = config_get_int_value(cfg_file, "PUERTO");
    cfg->TIEMPO_SINCRONIZACION = config_get_long_value(cfg_file, "TIEMPO_SINCRONIZACION");
    cfg->BLOCKS = config_get_int_value(cfg_file, "BLOCKS");
    cfg->BLOCK_SIZE = config_get_int_value(cfg_file, "BLOCK_SIZE");

    char** posiciones_sabotaje = config_get_array_value(cfg_file, "POSICIONES_SABOTAJE");
    cfg->POSICIONES_SABOTAJE = extraer_posiciones(posiciones_sabotaje);
    config_free_array_value(&posiciones_sabotaje);

    log_info(logger, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg_file);

    return 1;
}

bool crear_servidor(int* fd, char* name) {
    char* puerto = string_itoa(cfg->PUERTO);
    *fd = iniciar_servidor(
            logger,
            name,
            "0.0.0.0",
            puerto
    );
    free(puerto);
    if (*fd != -1) log_info(logger, "Server listo en IMS");
    return *fd != -1;
}

void cerrar_programa() {
    log_info(logger, "Cerrando programa");
    munmap(mem_map, superbloque->tamanio_fs);
    log_destroy(logger);
    bitarray_destroy(superbloque->bitarray);
    free(superbloque);
    free(cfg->PUNTO_MONTAJE);
    list_destroy_and_destroy_elements(cfg->POSICIONES_SABOTAJE, free_t_posicion);
    free(cfg);
    liberar_conexion(&server_fd);
}
