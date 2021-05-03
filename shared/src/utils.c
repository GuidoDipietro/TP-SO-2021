#include "../include/utils.h"

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

bool config_has_all_properties(t_config* cfg, char** properties) {
    for(uint8_t i = 0; properties[i] != NULL; i++)
        if(!config_has_property(cfg, properties[i]))
            return false;

    return true;
}