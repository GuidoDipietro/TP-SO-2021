#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>

NIVEL* iniciar_ventana(t_log*, char*);
int posicion_random(int);

#endif
