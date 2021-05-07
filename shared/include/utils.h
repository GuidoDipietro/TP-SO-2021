#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "protocolo.h"
#include "frees.h"

uint16_t string_split_len(char**);
bool config_has_all_properties(t_config*, char**);
t_list* extraer_posiciones(char**);
bool string_is_number(char*);

#endif