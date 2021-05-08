#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>

typedef struct {
    uint8_t x;
    uint8_t y;
} posicion;

NIVEL* iniciar_gui(t_log*, char*);
void crear_tripulantes(NIVEL*, uint8_t, t_list*);

#endif
