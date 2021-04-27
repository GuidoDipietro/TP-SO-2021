#include "../include/utils.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdint.h>

void string_split_free(char*** var) {
    char** arr = *var;

    for(uint8_t i = 0; arr[i] != NULL; i++)
        free(arr[i]);
    free(*var);
}

uint16_t string_split_len(char** arr) {
    uint8_t i = 0;
    while(arr[i] != NULL)
        i++;
    return i;
}