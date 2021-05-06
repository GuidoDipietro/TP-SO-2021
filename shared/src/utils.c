#include "../include/utils.h"

static bool solo_numeros(char* str) {
    uint8_t i = 0;
    while(str[i] != '\0') {
        if(str[i] < 48 || str[i] > 57)
            return false;
        i++;
    }
    return true;
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

// El array tiene que estar terminado en NULL
t_list* extraer_posiciones(char** str_posiciones) {
    t_list* lista = list_create();
    int i = 0;

    while(str_posiciones[i] != NULL) {
        char** pos_arr = string_split(str_posiciones[i], "|");

        if(
                string_split_len(pos_arr) != 2 ||
                !(solo_numeros(pos_arr[0]) && solo_numeros(pos_arr[1]))
        ) {
            list_destroy(lista);
            string_split_free(&pos_arr);
            list_destroy_and_destroy_elements(lista, free_t_posicion);
            return NULL;
        }

        t_posicion* pos = malloc(sizeof(t_posicion));
        pos->x = atoi(pos_arr[0]);
        pos->y = atoi(pos_arr[1]);
        list_add(lista, pos);

        string_split_free(&pos_arr);
        i++;
    }

    return lista;
}