#ifndef INTERFAZ_MEMORIA_H_
#define INTERFAZ_MEMORIA_H_

#include <commons/collections/list.h>
#include <commons/string.h>

#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>

#include "init_mrhq.h"
#include "manejo_memoria.h"
#include "monitor_tablas.h"
#include "estructuras.h"                            // s
#include "../../shared/include/structures.h"        // i
#include "../../shared/include/utils.h"
#include "../../shared/include/frees.h"

bool iniciar_patota_en_mp(uint32_t n_tripulantes, char* tareas, t_list* posiciones);
bool iniciar_tripulante_en_mp(uint32_t tid, uint32_t pid);
bool borrar_tripulante_de_mp(uint32_t id_tripulante);
t_tarea* fetch_tarea(uint32_t tid);

#endif