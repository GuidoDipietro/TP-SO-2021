#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include "../../shared/include/protocolo.h"
#include <curses.h>

void iniciar_gui(char*);
int crear_tripulantes(uint32_t, t_list*);
int expulsar_tripulante(uint32_t);
void chequear_errores(int);
int mover_tripulante(uint32_t, t_posicion*);

NIVEL* among_nivel;

#endif
