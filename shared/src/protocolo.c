#include "../include/protocolo.h"

static op_code recibir_cop(int fd) {
    op_code cop;
    if(recv(fd, &cop, sizeof(op_code), 0) != 0)
        return cop;
    else {
        close(fd);
        return -1;
    }    
}
void print_t_posicion(void* p) {
    t_posicion* t_p = (t_posicion*) p;
    printf("%d|%d\n", t_p->x, t_p->y);
}
void free_t_posicion(void* p) {
    free(p);
}

///

// EXPULSAR_TRIPULANTE //
// ATENCION_SABOTAJE //
// RESOLUCION_SABOTAJE //

bool recv_tripulante(int fd, uint8_t* id_tripulante) {
    void* stream = malloc(sizeof(uint8_t));
    if (recv(fd, stream, sizeof(uint8_t), 0) != sizeof(uint8_t))
        return false;
    deserializar_uint8_t(stream, id_tripulante);
    free(stream);
    return true;
}
bool send_tripulante(int fd, uint8_t id_tripulante, op_code cop) {
    void* stream = serializar_tripulante(id_tripulante, cop);
    if (send(fd, stream, sizeof(op_code)+sizeof(uint8_t), 0) == -1)
        return false;
    free(stream);
    return true;
}

void* serializar_tripulante(uint8_t id_tripulante, op_code cop) {
    void* stream = malloc(sizeof(op_code)+sizeof(uint8_t));
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &id_tripulante, sizeof(uint8_t));
    return stream;
}
void deserializar_uint8_t(void* stream, uint8_t* n) {
    memcpy(n, stream, sizeof(uint8_t));
}

// INICIAR_PATOTA //

bool recv_patota(int fd, uint8_t* n_tripulantes, char** filepath, t_list** posiciones) {
    // n_tripulantes
    void* stream = malloc(sizeof(uint8_t));
    if (recv(fd, stream, sizeof(uint8_t), 0) != sizeof(uint8_t))
        return false;
    deserializar_uint8_t(stream, n_tripulantes);

    // filepath
    uint8_t len_filepath;
    char* p_filepath;
        // len_filepath
    if (recv(fd, stream, sizeof(uint8_t), 0) != sizeof(uint8_t))
        return false;
    deserializar_uint8_t(stream, &len_filepath);
        // filepath en si
    free(stream);
    stream = malloc(len_filepath+1);
    if (recv(fd, stream, len_filepath+1, 0) != len_filepath+1)
        return false;
    deserializar_string(stream, &p_filepath, len_filepath);
    *filepath = p_filepath;

    // posiciones
    t_list* p_posiciones;
    free(stream);
    size_t sz_stream_posiciones = 2*sizeof(uint8_t)*(*n_tripulantes);
    stream = malloc(sz_stream_posiciones);
    if (recv(fd, stream, sz_stream_posiciones, 0) != sz_stream_posiciones)
        return false;
    p_posiciones = deserializar_t_list_posiciones(stream, *n_tripulantes);
    *posiciones = p_posiciones;

    free(stream);

    return true;
}
bool send_patota(int fd, uint8_t n_tripulantes, char* filepath, t_list* posiciones) {
    void* stream = serializar_iniciar_patota(n_tripulantes, filepath, posiciones);
    size_t sz_stream_posiciones = 2*sizeof(uint8_t)*list_size(posiciones); // 2 uint8_t por cada item
    size_t size = 
        sizeof(op_code)+                        // op_code
        sizeof(uint8_t)+                        // n_tripulantes
        sizeof(uint8_t)+strlen(filepath)+1+     // strlen(filepath) + filepath
        sz_stream_posiciones                    // stream_posiciones
    ;
    if (send(fd, stream, size, 0) == -1)
        return false;
    free(stream);
    return true;
}

static void* serializar_t_list_posiciones(t_list* lista) {
    void* stream = malloc(2*sizeof(uint8_t)*list_size(lista));
            // 2 uint8_t por cada elemento

    // serializo los elementos
    t_list_iterator* list_it = list_iterator_create(lista);
    for (uint8_t i=0; list_iterator_has_next(list_it); i+=2) {
        t_posicion* pos = list_iterator_next(list_it);
        memcpy(stream+(i+0)*sizeof(uint8_t), &pos->x, sizeof(uint8_t));
        memcpy(stream+(i+1)*sizeof(uint8_t), &pos->y, sizeof(uint8_t));
    }
    list_iterator_destroy(list_it);
    return stream;
}
t_list* deserializar_t_list_posiciones(void* stream, uint8_t n_elements) {
    t_list* lista = list_create();

    // De-serializo y los meto en la lista
    for (uint8_t i=0; n_elements>0; i+=2, n_elements--) {
        t_posicion* pos = malloc(sizeof(t_posicion));
        memcpy(&pos->x, stream+(i+0)*sizeof(uint8_t), sizeof(uint8_t));
        memcpy(&pos->y, stream+(i+1)*sizeof(uint8_t), sizeof(uint8_t));
        list_add(lista, pos);
    }
    return lista;
}
void* serializar_iniciar_patota(uint8_t n_tripulantes, char* filepath, t_list* posiciones) {
    op_code cop = INICIAR_PATOTA;

    uint8_t len_filepath = strlen(filepath);
    size_t sz_stream_posiciones = 2*sizeof(uint8_t)*list_size(posiciones); // 2 uint8_t por cada item

    void* stream_posiciones = serializar_t_list_posiciones(posiciones);
    void* stream = malloc(
        sizeof(op_code)+                        // op_code
        sizeof(uint8_t)+                        // n_tripulantes
        sizeof(uint8_t)+strlen(filepath)+1+     // strlen(filepath) + filepath
        sz_stream_posiciones                    // stream_posiciones
    );

    memcpy(stream, &cop, sizeof(op_code));

    memcpy(stream+sizeof(op_code), &n_tripulantes, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t), &len_filepath, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t)+sizeof(uint8_t), filepath, strlen(filepath)+1);
    memcpy(
        stream+sizeof(op_code)+sizeof(uint8_t)+sizeof(uint8_t)+strlen(filepath)+1,
        stream_posiciones,
        sz_stream_posiciones
    );
    // phew
    free(stream_posiciones);
    return stream;
}
void deserializar_string(void* stream, char** str, uint8_t len) {
    char* out = malloc(len+1);
    memcpy(out, stream, len+1);
    *str = out;
}
// void deserializar_iniciar_patota(void* stream, uint8_t* n_tripulantes, char** filepath, t_list** posiciones) {
//     memcpy(n_tripulantes, stream, sizeof(uint8_t));

//     uint8_t len_filepath;
//     memcpy(&len_filepath, stream+sizeof(uint8_t), sizeof(uint8_t));
//     char* p_filepath = malloc(len_filepath+1);
//     memcpy(p_filepath, stream+sizeof(uint8_t)+sizeof(uint8_t), len_filepath+1);
//     *filepath = p_filepath;

//     size_t sz_stream_posiciones = 2*sizeof(uint8_t)*(*n_tripulantes);
//     void* stream_posiciones = malloc(sz_stream_posiciones);
//     memcpy(
//         stream_posiciones,
//         stream+sizeof(uint8_t)+sizeof(uint8_t)+len_filepath+1,
//         sz_stream_posiciones
//     );
//     t_list* p_posiciones = deserializar_t_list_posiciones(stream_posiciones, *n_tripulantes);
//     *posiciones = p_posiciones;
//     free(stream_posiciones);
// }

// INICIAR_SELF_EN_PATOTA //

void* serializar_iniciar_self_en_patota(uint8_t id_tripulante, uint8_t id_patota) {
    op_code cop = INICIAR_SELF_EN_PATOTA;
    void* stream = malloc(sizeof(op_code)+sizeof(uint8_t));

    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &id_tripulante, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t), &id_patota, sizeof(uint8_t));
    return stream;
}
void deserializar_iniciar_self_en_patota(void* stream, uint8_t* id_tripulante, uint8_t* id_patota) {
    memcpy(id_tripulante, stream, sizeof(uint8_t));
    memcpy(id_patota, stream+sizeof(uint8_t), sizeof(uint8_t));
}


// faltan

/// 

// Ejemplo de implementacion del RECV en MRH:
// bool recv_mi_ram_hq(int fd) {
//     op_code cop = recibir_cop(fd);
//     switch (cop) {
//         case EXPULSAR_TRIPULANTE:
//         {
//             uint8_t id_tripulante;
//             void* stream = NULL;// cambiar por recibir el payload
//             deserializar_tripulante(stream, &id_tripulante);
//             // hacer cosas con eso
//         }
//             break;
//         case INICIAR_PATOTA:
//         {
//             uint8_t n_tripulantes;
//             char* filepath;
//             t_list* posiciones;
//             void* stream = NULL;// cambiar por recibir el payload
//             deserializar_iniciar_patota(stream, &n_tripulantes, &filepath, &posiciones);
//             // hacer cosas con eso
//         }
//             break;
//         // etc.
//         case -1:
//             puts("sedesconecto este loko");
//             close(fd);
//             // cosas
//             break;
//         default:
//             puts("no no, problemas\n");
//             // cosas
//             break;
//     }
// }