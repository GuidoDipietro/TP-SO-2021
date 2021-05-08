#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include "../../shared/include/protocolo.h"
#include <curses.h>

NIVEL* iniciar_gui(t_log*, char*);
void crear_tripulantes(NIVEL*, uint8_t, t_list*);

#endif
