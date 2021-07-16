#include "../include/utils_fs.h"

char* concatenar_montaje(char* str) {
    char* path = strdup(cfg->PUNTO_MONTAJE);
    string_append(&path, str);
    return path;
}

char* concatenar_montaje_files(char* filename) {
    char* path = strdup(cfg->PUNTO_MONTAJE);
    string_append(&path, "Files/");
    string_append(&path, filename);
    return path;
}

char* concatenar_montaje_bitacora(char* filename) {
    char* path = strdup(cfg->PUNTO_MONTAJE);
    string_append(&path, "Bitacoras/");
    string_append(&path, filename);
    return path;
}