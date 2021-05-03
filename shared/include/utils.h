#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/config.h>

void string_split_free(char***);
uint16_t string_split_len(char**);
bool config_has_all_properties(t_config*, char**);

#endif