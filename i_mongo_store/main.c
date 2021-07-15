#include "include/main.h"

static t_config_ims* initialize_cfg() {
    t_config_ims* cfg = malloc(sizeof(t_config_ims));
    cfg->PUNTO_MONTAJE = NULL;
    cfg->POSICIONES_SABOTAJE = NULL;
    return cfg;
}

t_log* logger;
t_list* OPEN_FILES;

int main() {
    cfg = initialize_cfg();
    OPEN_FILES = list_create();
    logger = log_create("i_mongo_store.log", MODULENAME, true, LOG_LEVEL_INFO);

    int server_fd;

    if(!cargar_configuracion() || !crear_servidor(&server_fd, SERVERNAME)) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    cargar_superbloque();
    cargar_bloques();
    iniciar_sincronizador();

    //crear_archivo("Oxigeno.ims", 'O');
    cargar_archivo("Oxigeno.ims");
    open_file_t* file = obtener_archivo("Oxigeno.ims");


    //char* content = "abcde";
    //write_to_file(file, content, 6);
    //print_open_file_t(file);
    //escribir_archivo("Oxigeno.ims", file->file);
    //print_open_file_t(file);

    /*void* content = malloc(sizeof(7));
    uint64_t* src = malloc(sizeof(uint64_t));
    *src = 0x123456789ABCDE00;
    memcpy(content, src, 7);
    write_to_file(file, content, 7);
    escribir_archivo("Oxigeno.ims", file->file);*/

    //char* b = malloc(4);
    //fwrite(b, sizeof(char), 4, mem_cpy);
    //memcpy(b, mem_cpy, 4);
    //printf("\n@@ %s @@", b);

    char* content = recuperar_archivo(file);
    printf("\n## %s ##\n", content);

    // Envio y recepcion de mensajes perenne
    while (server_escuchar(SERVERNAME, server_fd));

    cerrar_programa();

    return EXIT_SUCCESS;
}
