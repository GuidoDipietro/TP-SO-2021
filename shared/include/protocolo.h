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
#include <commons/log.h>
#include "utils.h"
#include "frees.h"

///

typedef enum {
    EXPULSAR_TRIPULANTE = 1,
    INICIAR_PATOTA,
    INICIAR_SELF_EN_PATOTA,
    SOLICITAR_TAREA,
    TAREA,
    MOVIMIENTO,
    OBTENER_BITACORA,
    BITACORA,
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

typedef enum {
    GENERAR_COMIDA_T    =14,
    CONSUMIR_COMIDA_T   =15,
    GENERAR_OXIGENO_T   =16,
    CONSUMIR_OXIGENO_T  =17,
    GENERAR_BASURA_T    =18,
    DESCARTAR_BASURA_T  =19,
    OTRO_T              =20,
} tipo_tarea;

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

// no se bien donde meter esto (me gustaria en utils y frees)
// hay que cambiar los makefiles porque en shared/
// si usas funciones de otros archivos de shared/ no compila
void print_t_posicion(void* p);
t_tarea* tarea_create(char* nombre, uint16_t param, t_posicion* pos, uint16_t dur, char* tipo);
void free_t_tarea(void* tarea);
void print_t_tarea(void* t);

//////

// GUIDO ESTUVO AQUI

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //
//////  UN SERIALIZAR Y UN DESERIALIZAR POR CADA MENSAJE   //////
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //
////// Algunos no necesitan enviar nada en el payload,     //////
////// asi que esos no tienen un serializar/deserializar   //////
////// por implementar. Otros comparten el deserializar    //////
////// con algun otro mensaje. Etc. Diviertanse.           //////
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //

////// FORMATO GENERAL //////
/// Serializar
// void* serializar_cosa(...variables);
//          -> si es de longitud variable comienza con size_t* size para conocer el tamanio post-serializar
//              ej.: void* serializar_cadena(size_t* size, char* cadena);
//
/// Deserializar
// void deserializar_cosa(void* stream, ...punteros a variables);
//          -> args de longitud variable llevan a la derecha su tamanio
//              salvo que se pueda determinar el tamanio de otra forma
//              ej.: deserializar_cadena(void* stream, char** cadena, size_t size_cadena);
//
/// Send
// bool send_cosa(int fd, ...variables); -> exito? true : false;
//
/// Recv
// bool recv_cosa(int fd, ...punteros a variables); -> exito? true : false;
//////

//////////// MENSAJES /////////////

// EXPULSAR_TRIPULANTE
// ATENCION_SABOTAJE
// RESOLUCION_SABOTAJE
bool recv_tripulante(int fd, uint8_t* id_tripulante);
bool send_tripulante(int fd, uint8_t id_tripulante, op_code cop);
void* serializar_tripulante(uint8_t id_tripulante, op_code cop);
void deserializar_uint8_t(void* stream, uint8_t* n);

// INICIAR_PATOTA //
// void dump_archivo(FILE* f);
bool recv_patota(int fd, uint8_t* n_tripulantes, t_list** tareas, t_list** posiciones);
bool send_patota(int, uint8_t, void*, size_t, t_list*);
void* serializar_contenido_archivo(size_t*, char*, t_log*);

////// faltan

// INICIAR_SELF_EN_PATOTA
bool send_iniciar_self_en_patota(int fd, uint8_t id_tripulante, uint8_t id_patota);
bool recv_iniciar_self_en_patota(int fd, uint8_t* id_tripulante, uint8_t* id_patota);

// SOLICITAR_TAREA //
bool send_solicitar_tarea(int fd);          //Tripulante: hey! quiero una tarea!
bool send_tarea(int fd, t_tarea* tarea);    //MRH: toma tu tarea
bool recv_tarea(int fd, t_tarea** tarea);   //Tripulante: yuhu me llego tu tarea

// MOVIMIENTO //
bool send_movimiento(int fd, uint8_t id_t, t_posicion* origen, t_posicion* destino);
bool recv_movimiento(int fd, uint8_t* id_t, t_posicion** origen, t_posicion** destino);

// como enviamos/recibimos la bitacora? una gran cadena con los datos para printear o algo?
// ver
// OBTENER_BITACORA //
bool send_obtener_bitacora(int fd, uint8_t id_tripulante);  //kiero la vitacoras deltripulante 69
// el recv de esto ^ deberia ser algo tipo
// case OBTENER_BITACORA:; uint8_t id_tripulante; recv_tripulante(fd, &id_tripulante);
bool send_bitacora(int fd, char* bitacora);                 //toma
bool recv_bitacora(int fd, char** bitacora);                //me llegouna vitacoras

// SABOTAJE //
bool send_sabotaje(int fd, t_posicion* posicion);
bool recv_sabotaje(int fd, t_posicion** posicion);

// Generico
bool send_codigo_op();

// no necesita serializar
// FIN_FSCK //
bool send_fin_fsck(int fd); // solo op code

// no necesita serializar
// INICIAR_FSCK //
bool send_iniciar_fsck(int fd); // solo op code

// INICIO_TAREA //
// FIN_TAREA //
bool send_solicitar_tarea(int fd);
bool send_tarea(int fd, t_tarea* t);
bool recv_tarea(int fd, t_tarea** t);

bool send_inicio_tarea(int fd, uint8_t id_tripulante, char* nombre_tarea);
bool send_fin_tarea(int fd, uint8_t id_tripulante, char* nombre_tarea);
bool recv_tripulante_nombretarea(int fd, uint8_t* id_tripulante, char** nombre_tarea);

// GENERAR //
// CONSUMIR //
// faltan
bool send_generar_consumir(int fd, char* item, uint16_t cant, op_code cop);
bool recv_item_cantidad(int fd, char** item, uint16_t* cant);
static void* serializar_generar(size_t* size, char* item, uint16_t cant);
static void* serializar_consumir(size_t* size, char* item, uint16_t cant);
static void deserializar_item_cantidad(void* stream, char** item, uint16_t* cant);

// no necesita serializar
// DESCARTAR_BASURA //
bool send_descartar_basura(int fd); // solo op code

// tengo suenio

#endif