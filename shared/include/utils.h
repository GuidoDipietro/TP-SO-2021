#ifndef UTILS_H_
#define UTILS_H_

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "structures.h"
#include "protocolo.h"
#include "frees.h"

uint16_t string_split_len(char**);
bool config_has_all_properties(t_config*, char**);
t_list* extraer_posiciones(char**);
bool string_is_number(char*);
t_tarea* tarea_string_to_t_tarea(char*);
t_list* raw_tareas_to_list(char*);
char* leer_archivo_completo(char*);

void ciclo();

char F(); // por si hay que poner F en el chat

#endif
