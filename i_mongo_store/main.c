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
    set_signal_handlers();

    // Envio y recepcion de mensajes perenne
    while (server_escuchar(SERVERNAME, server_fd));

    return EXIT_SUCCESS;
}

/*

CODIGO DE COMO OPERAR CON ARCHIVOS
LO GUARDO POR LAS DUDAS PARA DESPUES

crear_archivo("Oxigeno.ims", 'O');
cargar_archivo("Oxigeno.ims");
open_file_t* file = obtener_archivo("Oxigeno.ims");

char* content1 = "NOSE NI IDEA LA VERDAD CRACK";
write_to_file(file, content1, strlen(content1));
escribir_archivo("Oxigeno.ims", file->file);

crear_archivo("Pruebas.ims", 'R');
cargar_archivo("Pruebas.ims");
open_file_t* file2 = obtener_archivo("Pruebas.ims");
char* ctnt = "PRUEBAAA";
write_to_file(file2, ctnt, strlen(ctnt));
escribir_archivo("Pruebas.ims", file2->file);

char* content2 = "No creo la verdad";
write_to_file(file, content2, strlen(content2));
print_open_file_t(file);
escribir_archivo("Oxigeno.ims", file->file);
print_open_file_t(file);

char* rec1 = recuperar_archivo(file);
fwrite(rec1, (file->file)->size, 1, stdout);
fflush(stdout);
printf("\n");
char* rec2 = recuperar_archivo(file2);
fwrite(rec2, (file2->file)->size, 1, stdout);
fflush(stdout);

 tarea_generar("Oxigeno.ims", 'O', 25);
open_file_t* file = cargar_archivo("Oxigeno.ims");
consumir_recurso(file, 19);

*/