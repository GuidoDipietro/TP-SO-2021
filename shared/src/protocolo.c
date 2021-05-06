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

///

// EXPULSAR_TRIPULANTE //

void* serializar_expulsar_tripulante(uint8_t id_tripulante) {
    op_code cop = EXPULSAR_TRIPULANTE;
    void* stream = malloc(sizeof(op_code)+sizeof(uint8_t));
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &id_tripulante, sizeof(uint8_t));
    return stream;
}
void deserializar_tripulante(void* stream, uint8_t* id_tripulante) {
    memcpy(id_tripulante, stream, sizeof(uint8_t));
}

// INICIAR_PATOTA //

static void* serializar_t_list_posiciones(t_list* lista) {
    uint16_t n_elements = list_size(lista);
    void* stream = malloc(sizeof(uint16_t) + 2*sizeof(uint8_t)*n_elements);
            // header + 2 uint8_t por cada elemento

    memcpy(stream, &n_elements, sizeof(uint16_t)); // nro de elementos
    // serializo los elementos
    t_list_iterator* list_it = list_iterator_create(lista);
    for (uint8_t i=0; list_iterator_has_next(list_it); i+=2) {
        t_posicion* pos = list_iterator_next(list_it);
        memcpy(sizeof(uint16_t)+stream+(i+0)*sizeof(uint8_t), &pos->x, sizeof(uint8_t));
        memcpy(sizeof(uint16_t)+stream+(i+1)*sizeof(uint8_t), &pos->y, sizeof(uint8_t));
    }
    list_iterator_destroy(list_it);
    return stream;
}
static t_list* deserializar_t_list_posiciones(void* stream) {
    t_list* lista = list_create();
    uint16_t n_elements;
    // Cuantos son?
    memcpy(&n_elements, stream, sizeof(uint16_t));

    // De-serializo y los meto en la lista
    for (uint8_t i=0; n_elements>0; i+=2, n_elements--) {
        t_posicion* pos = malloc(sizeof(t_posicion));
        memcpy(&pos->x, sizeof(uint16_t)+stream+(i+0)*sizeof(uint8_t), sizeof(uint8_t));
        memcpy(&pos->y, sizeof(uint16_t)+stream+(i+1)*sizeof(uint8_t), sizeof(uint8_t));
        list_add(lista, pos);
    }
    return lista;
}
void* serializar_iniciar_patota(uint8_t n_tripulantes, char* filepath, t_list* posiciones) {
    op_code cop = INICIAR_PATOTA;

    uint8_t len_filepath = strlen(filepath);
    size_t sz_stream_posiciones =
        sizeof(uint16_t)+                       // nro de elementos
        2*sizeof(uint8_t)*list_size(posiciones) // 2 uint8_t por cada item
    ;
    void* stream_posiciones = serializar_t_list_posiciones(posiciones);
    void* stream = malloc(
        sizeof(op_code)+                        // op_code
        sizeof(uint8_t)+                        // n_tripulantes
        sizeof(uint8_t)+strlen(filepath)+1+     // strlen(filepath) + filepath
        sizeof(size_t)+
        sz_stream_posiciones                    // stream_posiciones
    );

    memcpy(stream, &cop, sizeof(op_code));

    memcpy(stream+sizeof(op_code), &n_tripulantes, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t), &len_filepath, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t)+sizeof(uint8_t), filepath, strlen(filepath)+1);
    memcpy(
        stream+sizeof(op_code)+sizeof(uint8_t)+sizeof(uint8_t)+strlen(filepath)+1,
        &sz_stream_posiciones,
        sizeof(size_t)
    );
    memcpy(
        stream+sizeof(op_code)+sizeof(uint8_t)+sizeof(uint8_t)+strlen(filepath)+1+sizeof(size_t),
        stream_posiciones,
        sz_stream_posiciones
    );
    // phew
    free(stream_posiciones);
    return stream;
}
void deserializar_iniciar_patota(void* stream, uint8_t* n_tripulantes, char** filepath, t_list** posiciones) {
    memcpy(n_tripulantes, stream, sizeof(uint8_t));

    uint8_t len_filepath;
    memcpy(&len_filepath, stream+sizeof(uint8_t), sizeof(uint8_t));
    char* p_filepath = malloc(len_filepath+1);
    memcpy(p_filepath, stream+sizeof(uint8_t)+sizeof(uint8_t), len_filepath+1);
    *filepath = p_filepath;

    size_t sz_stream_posiciones;
    memcpy(&sz_stream_posiciones, stream+sizeof(uint8_t)+sizeof(uint8_t)+len_filepath+1, sizeof(size_t));
    void* stream_posiciones = malloc(sz_stream_posiciones);
    memcpy(
        stream_posiciones,
        stream+sizeof(uint8_t)+sizeof(uint8_t)+len_filepath+1+sizeof(size_t),
        sz_stream_posiciones
    );
    t_list* p_posiciones = deserializar_t_list_posiciones(stream_posiciones);
    *posiciones = p_posiciones;
    free(stream_posiciones);
}


// faltan

/// 

// Ejemplo de implementacion del RECV en MRH:
bool recv_mi_ram_hq(int fd) {
    op_code cop = recibir_cop(fd);
    switch (cop) {
        case EXPULSAR_TRIPULANTE:
        {
            uint8_t id_tripulante;
            void* stream = NULL;// cambiar por recibir el payload
            deserializar_tripulante(stream, &id_tripulante);
            // hacer cosas con eso
        }
            break;
        case INICIAR_PATOTA:
        {
            uint8_t n_tripulantes;
            char* filepath;
            t_list* posiciones;
            void* stream = NULL;// cambiar por recibir el payload
            deserializar_iniciar_patota(stream, &n_tripulantes, &filepath, &posiciones);
            // hacer cosas con eso
        }
            break;
        // etc.
        case -1:
            puts("sedesconecto este loko");
            close(fd);
            // cosas
            break;
        default:
            puts("no no, problemas\n");
            // cosas
            break;
    }
}