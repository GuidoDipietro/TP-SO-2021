#include "../include/monitor_lista_archivos.h"

pthread_mutex_t MUTEX_LISTA_ARCHIVOS;

void agregar_archivo(open_file_t* file_data) {
    pthread_mutex_lock(&MUTEX_LISTA_ARCHIVOS);
    list_add(OPEN_FILES, file_data);
    pthread_mutex_unlock(&MUTEX_LISTA_ARCHIVOS);
}

static char* filename;
static bool filtar_archivo_por_nombre(open_file_t* file_data) {
    if(strlen(filename) != strlen(file_data->nombre))
        return false;

    return (bool) strcmp(file_data->nombre, filename) == 0;
}

open_file_t* obtener_archivo(char* nombre) {
    pthread_mutex_lock(&MUTEX_LISTA_ARCHIVOS);
    filename = nombre;
    open_file_t* ret = list_find(OPEN_FILES, (void*) filtar_archivo_por_nombre);

    if(ret == NULL)
        return NULL;
    
    (ret->refs)++;
    pthread_mutex_unlock(&MUTEX_LISTA_ARCHIVOS);
    return ret;
}

void monitor_iterar_lista_archivos(void (*f)(open_file_t*)) {
    pthread_mutex_lock(&MUTEX_LISTA_ARCHIVOS);
    list_iterate(OPEN_FILES, (void*) f);
    pthread_mutex_unlock(&MUTEX_LISTA_ARCHIVOS);
}