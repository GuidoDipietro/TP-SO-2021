#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/collections/list.h>

//////

typedef enum {
    EXPULSAR_TRIPULANTE=1,
    INICIAR_PATOTA,
    INICIAR_SELF_EN_PATOTA,
    SOLICITAR_TAREA,
    MOVIMIENTO,
    OBTENER_BITACORA,
    SABOTAJE,
    FIN_FSCK,
    INICIO_FSCK,
    INICIO_TAREA,
    FIN_TAREA,
    ATENCION_SABOTAJE,
    RESOLUCION_SABOTAJE,
    GENERAR,
    CONSUMIR,
    DESCARTAR_BASURA,
} op_code;

//////

typedef struct {
    uint8_t x;
    uint8_t y;
} t_posicion;

typedef struct {
    char* nombre;
    t_list* params;
    t_posicion* pos;
    uint16_t duracion;
} t_tarea;

// no se si los fd son uint32_t o que son, los dejo como 'int' por ahora
static op_code recibir_cop(int fd);
void print_t_posicion(void* p);
void free_t_posicion(void* p);

// GUIDO ESTUVO AQUI

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //
//////  UN SERIALIZAR Y UN DESERIALIZAR POR CADA MENSAJE   //////
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //
////// Algunos no necesitan enviar nada en el payload,     //////
////// asi que esos no tienen un serializar/deserializar   //////
////// por implementar. Otros comparten el deserializar    //////
////// con algun otro mensaje. Etc. Diviertanse.           //////
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //

// EXPULSAR_TRIPULANTE //
// ATENCION_SABOTAJE //
// RESOLUCION_SABOTAJE //
bool recv_tripulante(int fd, uint8_t* id_tripulante);
bool send_tripulante(int fd, uint8_t id_tripulante, op_code cop);
void* serializar_tripulante(uint8_t id_tripulante, op_code cop);
void deserializar_uint8_t(void* stream, uint8_t* id_tripulante);

// INICIAR_PATOTA //
bool send_patota(int fd, uint8_t n_tripulantes, char* filepath, t_list* posiciones);
bool recv_patota(int fd, uint8_t* n_tripulantes, char** filepath, t_list** posiciones);
static void* serializar_t_list_posiciones(t_list*);
void* serializar_iniciar_patota(uint8_t n_tripulantes, char* filepath, t_list* posiciones);
// void deserializar_iniciar_patota(void* stream, uint8_t* n_tripulantes, char** filepath, t_list** posiciones);
void deserializar_string(void* stream, char** str, uint8_t len);
t_list* deserializar_t_list_posiciones(void* stream, uint8_t n_elements);

////// faltan

// INICIAR_SELF_EN_PATOTA //
void* serializar_iniciar_self_en_patota(uint8_t id_tripulante, uint8_t id_patota);
void deserializar_iniciar_self_en_patota(void* stream, uint8_t* id_tripulante, uint8_t* id_patota);

// SOLICITAR_TAREA //
void* serializar_solicitar_tarea(t_tarea* tarea);
void deserializar_solicitar_tarea(void* stream, t_tarea** tarea);

// MOVIMIENTO //
void* serializar_movimiento(uint8_t id_tripulante, t_posicion* origen, t_posicion* destino);
void deserializar_movimiento(void* stream, uint8_t* id_tripulante, t_posicion** origen, t_posicion** destino);

// como enviamos/recibimos la bitacora? una gran cadena con los datos para printear o algo?
// ver
// OBTENER_BITACORA //
void* serializar_obtener_bitacora(uint8_t id_tripulante, void* bitacora);
void deserializar_obtener_bitacora(void* stream, uint8_t* tripulante, void** bitacora);

// SABOTAJE //
void* serializar_sabotaje(t_posicion* posicion);
void deserializar_sabotaje(void* stream, t_posicion** posicion);

// no necesita serializar
// FIN_FSCK //
void* serializar_fin_fsck(); // solo op_code

// no necesita serializar
// INICIAR_FSCK //
void* serializar_iniciar_fsck(); // solo op_code

// INICIO_TAREA //
// FIN_TAREA //
void* serializar_inicio_tarea(uint8_t id_tripulante, t_tarea* tarea);
void* serializar_fin_tarea(uint8_t id_tripulante, t_tarea* tarea);
void deserializar_tarea(void* stream, uint8_t* id_tripulante, t_tarea** tarea);

// GENERAR //
// CONSUMIR //
void* serializar_generar(char* item, uint16_t cant);
void* serializar_consumir(char* item, uint16_t cant);
void deserializar_item_cantidad(void* stream, char** item, uint16_t* cant);

// no necesita serializar
// DESCARTAR_BASURA //
void* serializar_descartar_basura(); // solo op_code

// tengo suenio

#endif