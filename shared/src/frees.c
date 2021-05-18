#include "../include/frees.h"

void string_split_free(char*** var) {
    char** arr = *var;

    for(uint8_t i = 0; arr[i] != NULL; i++)
        free(arr[i]);
    free(*var);
}

void config_free_array_value(char*** arr) {
    char** a = *arr;
    int i = 0;

    while(a[i] != NULL) {
        free(a[i]);
        i++;
    }

    free(a);
}

void free_t_posicion(void* p) {
    free(p);
}