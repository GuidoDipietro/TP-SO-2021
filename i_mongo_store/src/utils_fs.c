#include "../include/utils_fs.h"

char* concatenar_montaje(char* str) {
    char* path = strdup(cfg->PUNTO_MONTAJE);
    string_append(&path, str);
    return path;
}