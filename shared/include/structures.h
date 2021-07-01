#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <stdint.h>

typedef enum {
    NEW                 = 'N',
    READY               = 'R',
    EXEC                = 'X',
    BLOCKED             = 'B',
    BLOCKEDSAB          = 'S',
    EXIT                = 'E',
} t_status;

typedef enum {
    GENERAR_COMIDA_T    = 14,
    CONSUMIR_COMIDA_T   = 15,
    GENERAR_OXIGENO_T   = 16,
    CONSUMIR_OXIGENO_T  = 17,
    GENERAR_BASURA_T    = 18,
    DESCARTAR_BASURA_T  = 19,
    OTRO_T              = 20,
    NULL_T              = 69,
} tipo_tarea;

typedef enum {
    OXIGENO             =  8,
    COMIDA              = 14,
    BASURA              = 60,
} tipo_item;

typedef struct {
    uint8_t x;
    uint8_t y;
} t_posicion;

typedef struct {
    char* nombre;
    uint16_t param;
    t_posicion* pos;
    uint16_t duracion;
    tipo_tarea tipo;
} t_tarea;

#endif