#ifndef INTERFAZ_MEMORIA_H_
#define INTERFAZ_MEMORIA_H_

#include <commons/collections/list.h>

#include <stdlib.h>
#include <stdbool.h>

#include "init_mrhq.h"
#include "estructuras.h"
#include "manejo_memoria.h"
#include "monitor_tablas.h"

bool iniciar_patota_en_mp(char* tareas, t_list* posiciones);
bool iniciar_tripulante_en_mp(uint32_t tid, uint32_t pid);

#endif