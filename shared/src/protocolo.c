#include "../include/protocolo.h"

///

void print_t_posicion(void* p) {
    t_posicion* t_p = (t_posicion*) p;
    printf("%d|%d\n", t_p->x, t_p->y);
}

t_tarea* tarea_create(char* nombre, uint16_t param, t_posicion* pos, uint16_t dur, char* tipo) {
    t_tarea* tarea = malloc(sizeof(t_tarea));
    tarea->pos = malloc(sizeof(t_posicion));

    tarea->nombre = string_duplicate(nombre);
    tarea->param = param;
    tarea->pos->x = pos->x;
    tarea->pos->y = pos->y;
    tarea->duracion = dur;
    // doloroso
    tipo_tarea t;
    if (!strcmp(tipo, "GENERAR_COMIDA")) t = GENERAR_COMIDA_T;
    else if (!strcmp(tipo, "CONSUMIR_COMIDA")) t = CONSUMIR_COMIDA_T;
    else if (!strcmp(tipo, "GENERAR_OXIGENO")) t = GENERAR_OXIGENO_T;
    else if (!strcmp(tipo, "CONSUMIR_OXIGENO")) t = CONSUMIR_OXIGENO_T;
    else if (!strcmp(tipo, "GENERAR_BASURA")) t = GENERAR_BASURA_T;
    else if (!strcmp(tipo, "DESCARTAR_BASURA")) t = DESCARTAR_BASURA_T;
    else t = OTRO_T;
    tarea->tipo = t;

    return tarea;
}
void print_t_tarea(void* t) {
    t_tarea* tarea = (t_tarea*) t;
    printf("%s %d;%d;%d;%d;%d\n",
        tarea->nombre,
        tarea->param,
        tarea->pos->x,
        tarea->pos->y,
        tarea->duracion,
        tarea->tipo
    );
}
void free_t_tarea(void* t) {
    t_tarea* tarea = (t_tarea*) t;
    free(tarea->nombre);
    free_t_posicion(tarea->pos);
    free(tarea);
}

///

static op_code recibir_cop(int fd) {
    op_code cop;
    if(recv(fd, &cop, sizeof(op_code), 0) != 0)
        return cop;
    else {
        close(fd);
        return -1;
    }    
}

// EXPULSAR_TRIPULANTE //
// ATENCION_SABOTAJE //
// RESOLUCION_SABOTAJE //

bool recv_tripulante(int fd, uint8_t* id_tripulante) {
    void* stream = malloc(sizeof(uint8_t));
    if (recv(fd, stream, sizeof(uint8_t), 0) != sizeof(uint8_t)) {
        free(stream);
        return false;
    }
    deserializar_uint8_t(stream, id_tripulante);
    free(stream);
    return true;
}

bool send_tripulante(int fd, uint8_t id_tripulante, op_code cop) {
    size_t size = sizeof(op_code)+sizeof(uint8_t);
    void* stream = serializar_tripulante(id_tripulante, cop);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_tripulante(uint8_t id_tripulante, op_code cop) {
    size_t size = sizeof(op_code)+sizeof(uint8_t);
    void* stream = malloc(size);
    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &id_tripulante, sizeof(uint8_t));
    return stream;
}
void deserializar_uint8_t(void* stream, uint8_t* n) {
    memcpy(n, stream, sizeof(uint8_t));
}

// INICIAR_PATOTA //

// void dump_archivo(FILE* file) {
//     char* line = NULL;
//     size_t sz = 0;
//     size_t len = 0;

//     while ((len = getline(&line, &sz, file)) != -1) {
//         puts(line);
//     }
//     if (line) free(line);
//     puts("");
// }

bool recv_patota(int fd, uint8_t* n_tripulantes, t_list** tareas, t_list** posiciones) {
    // tamanio total del stream
    size_t size;
    if (recv(fd, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    // recibe TODO el stream
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    // desarmando el chorizo de bits
    char* r_tareas;         // el malloc lo realiza deserializar_iniciar_patota()
    t_list* r_posiciones;   // same
    deserializar_iniciar_patota(stream, n_tripulantes, &r_tareas, &r_posiciones);
    
    t_list* r_list_tareas = raw_tareas_to_list(r_tareas);
    *tareas = r_list_tareas;
    *posiciones = r_posiciones;

    free(stream);
    free(r_tareas);
    return true;
}

bool send_patota
(int fd, uint8_t n_tripulantes, void* s_tareas, size_t sz_s_tareas, t_list* posiciones) {
    size_t size;
    void* stream = serializar_iniciar_patota(&size, n_tripulantes, s_tareas, sz_s_tareas, posiciones);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

// flor de firma tiene esta func
static void* serializar_iniciar_patota
(size_t* size, uint8_t n_tripulantes, void* s_tareas, size_t sz_s_tareas, t_list* posiciones) {
    //// Stream lista posiciones
    size_t size_posiciones;
    void* stream_posiciones = serializar_t_list_posiciones(&size_posiciones, posiciones);

    //// Stream completo
    size_t size_total =
        sizeof(op_code)+                        // COP
        sizeof(size_t)+                         // size total del stream
        sizeof(uint8_t)+                        // n_tripulantes
        sizeof(size_t)+sz_s_tareas+             // size contenido archivo + contenido archivo
        sizeof(size_t)+size_posiciones          // size posiciones + posiciones
    ;
    void* stream = malloc(size_total);
    //// Payload (lo que sigue del COP)
    size_t size_payload = size_total-sizeof(op_code)-sizeof(size_t);
        //el tamanio del resto no incluye el cop ni el size!

    op_code cop = INICIAR_PATOTA;

    // no se asusten
    memcpy(stream, &cop, sizeof(op_code)); //cop
    memcpy(stream+sizeof(op_code), &size_payload, sizeof(size_t)); //size payload
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t),
        &n_tripulantes,
        sizeof(uint8_t)
    ); //n_tripulantes
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint8_t),
        &sz_s_tareas,
        sizeof(size_t)
    ); // size contenido archivo
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint8_t)+sizeof(size_t),
        s_tareas,
        sz_s_tareas
    ); // contenido archivo
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint8_t)+sizeof(size_t)+sz_s_tareas,
        &size_posiciones,
        sizeof(size_t)
    ); // size posiciones
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(uint8_t)+
        sizeof(size_t)+sz_s_tareas+sizeof(size_t),
        stream_posiciones,
        size_posiciones
    ); // posiciones

    // ya hicimos memcpy de esto asi que lo podemos liberar
    free(stream_posiciones);
    *size = size_total;

    return stream;
}
static void deserializar_iniciar_patota
(void* stream, uint8_t* n_tripulantes, char** tareas, t_list** posiciones) {
    size_t sz_tareas, sz_posiciones;
    // dont panic
    memcpy(n_tripulantes, stream, sizeof(uint8_t));             // n_tripulantes
    memcpy(&sz_tareas, stream+sizeof(uint8_t), sizeof(size_t)); //size contenido archivo

    char* p_tareas = malloc(sz_tareas);
    memcpy(p_tareas, stream+sizeof(uint8_t)+sizeof(size_t), sz_tareas); //contenido archivo
    *tareas = p_tareas;

    memcpy(
        &sz_posiciones,
        stream+sizeof(uint8_t)+sizeof(size_t)+sz_tareas,
        sizeof(size_t)
    ); // size posiciones

    void* stream_posiciones = malloc(sz_posiciones);
    memcpy(
        stream_posiciones,
        stream+sizeof(uint8_t)+sizeof(size_t)+sz_tareas+sizeof(size_t),
        sz_posiciones
    ); // posiciones
    t_list* lista = deserializar_t_list_posiciones(stream_posiciones, *n_tripulantes);
    *posiciones = lista;

    free(stream_posiciones);
}

void* serializar_contenido_archivo(size_t* size, char* path, t_log* logger) {
    FILE* file = fopen(path, "r+");

    if(file == NULL) {
        log_error(logger, "No se pudo abrir el archivo de tareas en %s", path);
        return NULL;
    }

    void* stream;
    bool first_malloc_done = false;

    char* line = NULL;
    size_t sz = 0;
    size_t len = 0;
    size_t total_len = 0;

    while ((len = getline(&line, &sz, file)) != -1) {
        // la linea leida tiene el formato COSASCOSAS\r\n
        // menos si es la ultima, es solo COSASCOSAS, por eso un \0 extra al final
        // se serializa eso tal cual para poder facilitar el procesamiento al recibirse

        if (!first_malloc_done) {
            stream = malloc(len);                       // reserva 'len' bytes
            first_malloc_done = true;
        }
        else stream = realloc(stream, total_len+len);   //reserva 'len' bytes mas que los que tiene

        memcpy(stream+total_len, line, len);
        total_len += len;
    }
    // Centinela al final (hecho asi por legibilidad)
    char centinela = '\0';
    stream = realloc(stream, total_len+sizeof(char));
    memcpy(stream+total_len, &centinela, sizeof(char)); // si ya se que sizeof(char) es 1

    if (line) free(line);

    *size = total_len+1;
    fclose(file);
    return stream;
}
static void deserializar_contenido_archivo(void* stream, char** out, size_t size) {
    char* buffer = malloc(size);
    memcpy(buffer, stream, size);
    *out = buffer;
}

static void* serializar_t_list_posiciones(size_t* size, t_list* lista) {
    *size = 2*sizeof(uint8_t)*list_size(lista); // 2 uint8_t por cada elemento
    void* stream = malloc(*size);

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
static t_list* deserializar_t_list_posiciones(void* stream, uint8_t n_elements) {
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

// INICIAR_SELF_EN_PATOTA //

bool send_iniciar_self_en_patota(int fd, uint8_t id_tripulante, uint8_t id_patota){
    size_t size = sizeof(op_code)+2*sizeof(uint8_t);
    void* stream = serializar_iniciar_self_en_patota(id_tripulante,id_patota);

    if(send(fd,stream,size,0) == -1){
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_iniciar_self_en_patota(int fd, uint8_t* id_tripulante, uint8_t* id_patota){
    size_t size = 2*sizeof(uint8_t);
    void* stream = malloc(size);

    if(recv(fd,stream,size,0) != size){
        free(stream);
        return false;
    }

    deserializar_iniciar_self_en_patota(stream,id_tripulante,id_patota);
    free(stream);
    return true;
}

void* serializar_iniciar_self_en_patota(uint8_t id_tripulante, uint8_t id_patota) {
    op_code cop = INICIAR_SELF_EN_PATOTA;
    void* stream = malloc(sizeof(op_code)+2*sizeof(uint8_t));

    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &id_tripulante, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t), &id_patota, sizeof(uint8_t));

    return stream;
}
void deserializar_iniciar_self_en_patota(void* stream, uint8_t* id_tripulante, uint8_t* id_patota) {
    memcpy(id_tripulante, stream, sizeof(uint8_t));
    memcpy(id_patota, stream+sizeof(uint8_t), sizeof(uint8_t));
}

// SABOTAJE //

bool send_sabotaje(int fd, t_posicion* posicion){
    size_t size = sizeof(op_code) + sizeof(t_posicion);
    void* stream = serializar_sabotaje(posicion);

    if(send(fd,stream,size,0) == -1){
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

bool recv_sabotaje(int fd, t_posicion** posicion){
    size_t size = sizeof(t_posicion);
    void* stream = malloc(size);

    if(recv(fd,stream,size,0) != size){
        free(stream);
        return false;
    }

    deserializar_sabotaje(stream,posicion);
    free(stream);
    return true;
}

static void* serializar_sabotaje(t_posicion* posicion){
    op_code cop = SABOTAJE;
    size_t size = sizeof(op_code) + sizeof(posicion);
    void* stream = malloc(size);

    memcpy(stream, &cop, sizeof(op_code));
    memcpy(stream+sizeof(op_code), &posicion->x, sizeof(uint8_t));
    memcpy(stream+sizeof(op_code)+sizeof(uint8_t), &posicion->y, sizeof(uint8_t));

    return stream;
}

static void deserializar_sabotaje(void* stream, t_posicion** posicion){
    t_posicion* r_pos = malloc(sizeof(t_posicion));

    memcpy(&r_pos->x, stream, sizeof(uint8_t));
    memcpy(&r_pos->y, stream+sizeof(uint8_t), sizeof(uint8_t));

    *posicion = r_pos;
}


// no necesita serializar

bool send_codigo_op(int fd, op_code cop){
    size_t size = sizeof(op_code);
    return send(fd,&cop,size,0) != -1;
}

// INICIAR_FSCK //
bool send_iniciar_fsck(int fd){
    return send_codigo_op(fd, INICIO_FSCK);
}

// FIN_FSCK //
bool send_fin_fsck(int fd){
    return send_codigo_op(fd, FIN_FSCK);
}

// TAREAS //
bool send_solicitar_tarea(int fd) {
    return send_codigo_op(fd, SOLICITAR_TAREA);
}

bool send_tarea(int fd, t_tarea* tarea) {
    size_t size;
    void* stream = serializar_tarea(&size, tarea);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}
bool recv_tarea(int fd, t_tarea** tarea) {
    size_t size;
    if (recv(fd, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    t_tarea* r_tarea;
    deserializar_tarea(stream, &r_tarea);
    *tarea = r_tarea;

    free(stream);
}

static void* serializar_tarea(size_t* size, t_tarea* tarea) {
    op_code cop = TAREA;
    size_t sz_nombre = strlen(tarea->nombre)+1;
    *size =
        sizeof(op_code)+        // cop
        sizeof(size_t)+         // tamanio payload
        sizeof(size_t)+         // tamanio nombre
        sz_nombre+              // nombre
        sizeof(uint16_t)+       // param
        sizeof(t_posicion)+     // posicion
        sizeof(uint16_t)+       // duracion
        sizeof(tipo_tarea)      // tipo
    ;
    size_t sz_payload = *size - sizeof(op_code) - sizeof(size_t);
    void* stream = malloc(*size);

    memcpy(stream, &cop, sizeof(op_code)); // cop
    memcpy(stream+sizeof(op_code), &sz_payload, sizeof(size_t)); // tamanio payload
    memcpy(stream+sizeof(op_code)+sizeof(size_t), &sz_nombre, sizeof(size_t)); // tamanio nombre
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t),
        tarea->nombre,
        sz_nombre
    ); // nombre
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t)+sz_nombre,
        &tarea->param,
        sizeof(uint16_t)
    ); // param
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t)+sz_nombre+sizeof(uint16_t),
        &tarea->pos->x,
        sizeof(uint8_t)
    ); // posicion x
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t)+sz_nombre+sizeof(uint16_t)+sizeof(uint8_t),
        &tarea->pos->y,
        sizeof(uint8_t)
    ); // posicion u
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t)+sz_nombre+sizeof(uint16_t)+2*sizeof(uint8_t),
        &tarea->duracion,
        sizeof(uint16_t)
    ); // duracion
    memcpy(
        stream+sizeof(op_code)+sizeof(size_t)+sizeof(size_t)+
        sz_nombre+sizeof(uint16_t)+2*sizeof(uint8_t)+sizeof(uint16_t),
        &tarea->tipo,
        sizeof(tipo_tarea)
    ); // tipo

    return stream;
}
static void deserializar_tarea(void* stream, t_tarea** tarea) {
    t_tarea* r_tarea = malloc(sizeof(t_tarea));
    r_tarea->pos = malloc(sizeof(t_posicion));

    // tamanio nombre
    size_t sz_nombre;
    memcpy(&sz_nombre, stream, sizeof(size_t));
    // nombre
    r_tarea->nombre = malloc(sz_nombre);
    memcpy(r_tarea->nombre, stream+sizeof(size_t), sz_nombre);
    // param
    memcpy(&r_tarea->param, stream+sizeof(size_t)+sz_nombre, sizeof(uint16_t));
    // posicion
    memcpy(
        &r_tarea->pos->x,
        stream+sizeof(size_t)+sz_nombre+sizeof(uint16_t),
        sizeof(uint8_t)
    );
    memcpy(
        &r_tarea->pos->y,
        stream+sizeof(size_t)+sz_nombre+sizeof(uint16_t)+sizeof(uint8_t),
        sizeof(uint8_t)
    );
    // duracion
    memcpy(
        &r_tarea->duracion,
        stream+sizeof(size_t)+sz_nombre+sizeof(uint16_t)+sizeof(uint8_t)+sizeof(uint8_t),
        sizeof(uint16_t)
    );
    // tipo
    memcpy(
        &r_tarea->tipo,
        stream+sizeof(size_t)+sz_nombre+sizeof(uint16_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint16_t),
        sizeof(tipo_tarea)
    );
    *tarea = r_tarea;
}

// faltan

/// 
