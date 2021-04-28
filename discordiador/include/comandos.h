#ifndef COMMANDOS_H_
#define COMANDOS_H_

#include <stdint.h>

typedef struct {
    uint8_t x;
    uint8_t y;
} posicionTripulante;

void iniciar_patota(char*);
void listar_tripulantes(char*);
void expulsar_tripulante(char*);
void iniciar_planificacion(char*);
void pausar_planificacion(char*);
void obtener_bitacora(char*);

#endif