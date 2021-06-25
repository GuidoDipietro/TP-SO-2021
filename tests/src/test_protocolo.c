#include "../include/test_protocolo.h"

#define IP "127.0.0.1"
#define PUERTO_BASE 6000 //por si tira el error de puerto abierto n stuff
//no se exactamente por que se produce pero a veces pasa

// Genera puertos distintos cada vez (creditos a criszkutnik)
static char* next_port() {
    static int n_port = PUERTO_BASE;
    char* out = malloc(5);
    sprintf(out, "%d", n_port++);
    return out;
}

// Sockets y logger
static t_log* logger;
static int server_fd;
static int cliente_fd;

// Funcion de setup (una vez antes de cada test)
void crear_conexiones() {
    char* puerto = next_port();

    logger = log_create("tests.log", "TESTS", true, LOG_LEVEL_INFO);
    server_fd = iniciar_servidor(
            logger,
            "TEST_SERVER",
            IP,
            puerto
    );
    if (server_fd == -1) {
        printf("Fallo al crear server\n");
        return;
    }

    cliente_fd = crear_conexion(
            logger,
            "TEST_SERVER",//nombre del server
            IP,
            puerto
    );
    if (cliente_fd == 0) {
        printf("Fallo al crear cliente\n");
        close(server_fd);
    }
    free(puerto);
}
// Funcion de teardown (una vez despues de cada test)
void cerrar_conexiones() {
    close(server_fd);
    close(cliente_fd);
    log_destroy(logger);
}

//// ACTUAL TESTS ////

// TESTS DE SEND-RECV CON DOS PROCESOS
void test_tripulante() {
    uint32_t id_tripulante=14, r_id_tripulante;

    // Inicializacion de semaforos compartidos (wtf? y bueno.)
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_tripulante(cliente_fd, id_tripulante, EXPULSAR_TRIPULANTE)) {
            log_error(logger, "Error enviando tripulante");
        }
        sem_post(sem_padre);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        if (!recv_tripulante(conexion_fd, &r_id_tripulante)) {
            log_error(logger, "Error recibiendo tripulante");
        }

        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
    }
}

void test_iniciar_self_en_patota() {
    uint32_t id_tripulante=69, r_id_tripulante;
    uint32_t id_patota=13, r_id_patota;

    // semaforos compartidos
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    pid_t pid = fork();

    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_iniciar_self_en_patota(cliente_fd, id_tripulante, id_patota)) {
            log_error(logger, "Error enviando inciar_self_en_patota");
        }
        sem_post(sem_padre);
        exit(0);
    }
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        if (!recv_iniciar_self_en_patota(conexion_fd, &r_id_tripulante, &r_id_patota)) {
            log_error(logger, "Error recibiendo tripulante");
        }

        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
        CU_ASSERT_EQUAL(id_patota, r_id_patota);
    }
}

static bool t_posicion_equals(t_posicion* p1, t_posicion* p2) {
    return p1->x==p2->x && p1->y==p2->y;
}

void test_sabotaje() {
    t_posicion* posicion = malloc(sizeof(t_posicion));
    posicion->x = 14; posicion->y = 27;

    t_posicion* r_posicion;

    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_sabotaje(cliente_fd, posicion)) {
            log_error(logger, "Error enviando tripulante");
        }
        sem_post(sem_padre);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        if (!recv_sabotaje(conexion_fd, &r_posicion)) {
            log_error(logger, "Error recibiendo tripulante");
        }

        CU_ASSERT_TRUE(t_posicion_equals(posicion, r_posicion));
    }
    free(posicion);
    free(r_posicion);
}

void test_send_cop() {
    op_code cop1 = INICIO_FSCK, cop2 = INICIAR_SELF_EN_PATOTA, cop3 = EXPULSAR_TRIPULANTE; //random
    op_code r_cop1, r_cop2, r_cop3;

    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_codigo_op(cliente_fd, cop1)) {
            log_error(logger, "Error enviando cop");
        }
        sem_post(sem_padre);

        sem_wait(sem_hijo);
        if (!send_codigo_op(cliente_fd, cop2)) {
            log_error(logger, "Error enviando cop");
        }
        sem_post(sem_padre);

        sem_wait(sem_hijo);
        if (!send_codigo_op(cliente_fd, cop3)) {
            log_error(logger, "Error enviando cop");
        }
        sem_post(sem_padre);

        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }

        if (recv(conexion_fd, &r_cop1, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop1 %d", r_cop1);
        }
        sem_post(sem_hijo);

        sem_wait(sem_padre);
        if (recv(conexion_fd, &r_cop2, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop2 %d", r_cop2);
        }
        sem_post(sem_hijo);

        sem_wait(sem_padre);
        if (recv(conexion_fd, &r_cop3, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop3 %d", r_cop3);
        }

        CU_ASSERT_EQUAL(cop1, r_cop1);
        CU_ASSERT_EQUAL(cop2, r_cop2);
        CU_ASSERT_EQUAL(cop3, r_cop3);
    }
}

void test_tarea() {
    t_posicion* pos = malloc(sizeof(t_posicion));
    pos->x = 6; pos->y = 9;
    t_tarea* tarea = tarea_create("Tarea GENERAR BASURA TEST", 14, pos, 73, "GENERAR_BASURA");
    free(pos);

    // Inicializacion de semaforos compartidos (wtf? y bueno.)
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_tarea(cliente_fd, tarea)) {
            log_error(logger, "Error enviando tarea");
        }
        sem_post(sem_padre);

        free_t_tarea(tarea);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }

        t_tarea* r_tarea;
        if (!recv_tarea(conexion_fd, &r_tarea)) {
            log_error(logger, "Error recibiendo tarea");
        }

        CU_ASSERT_TRUE(strcmp(tarea->nombre, r_tarea->nombre) == 0);
        CU_ASSERT_EQUAL(tarea->param, r_tarea->param);
        CU_ASSERT_TRUE(t_posicion_equals(tarea->pos, r_tarea->pos));
        CU_ASSERT_EQUAL(tarea->duracion, r_tarea->duracion);
        CU_ASSERT_EQUAL(tarea->tipo, r_tarea->tipo);

        free_t_tarea(tarea);
        free_t_tarea(r_tarea);
    }
}

void test_movimiento() {
    uint32_t id_tripulante = 32;
    t_posicion* origen = malloc(sizeof(t_posicion));
    origen->x = 7; origen->y = 66;
    t_posicion* destino = malloc(sizeof(t_posicion));
    destino->x = 22; destino->y = 29; // alto salto se mando

    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_movimiento(cliente_fd, id_tripulante, origen, destino)) {
            log_error(logger, "Error enviando movimiento");
        }
        sem_post(sem_padre);

        free_t_posicion(origen);
        free_t_posicion(destino);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }

        uint32_t r_id_tripulante;
        t_posicion* r_origen, *r_destino;
        if (!recv_movimiento(conexion_fd, &r_id_tripulante, &r_origen, &r_destino)) {
            log_error(logger, "Error recibiendo movimiento");
        }

        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
        CU_ASSERT_TRUE(t_posicion_equals(origen, r_origen) && t_posicion_equals(destino, r_destino));
        
        free_t_posicion(origen);
        free_t_posicion(destino);
        free_t_posicion(r_origen);
        free_t_posicion(r_destino);
    }
}

void test_bitacora() {
    char* bitacora = strdup("No tenemos idea como se va a mandar esto, pero podemos poner esta cadena para probar por el momento.");
    uint32_t id_tripulante = 13;

    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_obtener_bitacora(cliente_fd, id_tripulante)) {
            log_error(logger, "Error enviando obtener_bitacora");
        }
        sem_post(sem_padre);

        sem_wait(sem_hijo);
        if (!send_bitacora(cliente_fd, bitacora)) {
            log_error(logger, "Error enviando bitacora");
        }
        sem_post(sem_padre);
        free(bitacora);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }

        // cop OBTENER_BITACORA
        op_code cop1;
        if (recv(conexion_fd, &cop1, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "(1) Error recibiendo cop1 %d", cop1);
        }
        uint32_t r_id_tripulante;
        if (!recv_tripulante(conexion_fd, &r_id_tripulante)) {
            log_error(logger, "Error recibiendo id_tripulante de OBTENER_BITACORA");
        }
        sem_post(sem_hijo);

        // cop BITACORA + la bitacora a continuacion
        sem_wait(sem_padre);
        op_code cop2;
        if (recv(conexion_fd, &cop2, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "(2) Error recibiendo cop2 %d", cop2);
        }
        char* r_bitacora;
        if (!recv_bitacora(conexion_fd, &r_bitacora)) {
            log_error(logger, "Error recibiendo bitacora");
        }

        CU_ASSERT_EQUAL(cop1, OBTENER_BITACORA);
        CU_ASSERT_EQUAL(cop2, BITACORA);
        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
        CU_ASSERT_TRUE( strcmp(bitacora, r_bitacora) == 0 );

        free(bitacora);
        free(r_bitacora);
    }
}

void test_accion_tripulante_tarea() {
    char* nombre1 = strdup("Dormir la cafe con leche y tomarse una siesta");
    char* nombre2 = strdup("Irse de vacaciones a milanesa y comerse una Tuvalu");
    uint32_t id_tripulante1 = 49, id_tripulante2 = 27;

    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_inicio_tarea(cliente_fd, id_tripulante1, nombre1)) {
            log_error(logger, "Error enviando inicio_tarea");
        }
        sem_post(sem_padre);

        sem_wait(sem_hijo);
        if (!send_fin_tarea(cliente_fd, id_tripulante2, nombre2)) {
            log_error(logger, "Error enviando fin_tarea");
        }
        sem_post(sem_padre);

        free(nombre1); free(nombre2);

        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop1;
        if (recv(conexion_fd, &cop1, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop1 %d", cop1);
        }
        uint32_t r_id_tripulante1;
        char* r_nombre1;
        if (!recv_tripulante_nombretarea(conexion_fd, &r_id_tripulante1, &r_nombre1)) {
            log_error(logger, "Error recibiendo tripulante/nombre de inicio_tarea");
        }
        sem_post(sem_hijo);

        sem_wait(sem_padre);
        op_code cop2;
        if (recv(conexion_fd, &cop2, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop2 %d", cop2);
        }
        uint32_t r_id_tripulante2;
        char* r_nombre2;
        if (!recv_tripulante_nombretarea(conexion_fd, &r_id_tripulante2, &r_nombre2)) {
            log_error(logger, "Error recibiendo tripulante/nombre de fin_tarea");
        }

        CU_ASSERT_EQUAL(cop1, INICIO_TAREA);
        CU_ASSERT_EQUAL(cop2, FIN_TAREA);
        CU_ASSERT_EQUAL(id_tripulante1, r_id_tripulante1);
        CU_ASSERT_EQUAL(id_tripulante2, r_id_tripulante2);
        CU_ASSERT_TRUE(strcmp(nombre1, r_nombre1) == 0);
        CU_ASSERT_TRUE(strcmp(nombre2, r_nombre2) == 0);

        free(nombre1); free(r_nombre1); free(nombre2); free(r_nombre2);
    }
}

void test_generar_consumir_item() {
    // un test solo porque es literalmente la misma funcion para CONSUMIR cambiando un arg
    tipo_item item = OXIGENO;
    uint16_t cant = 10;

    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_generar_consumir(cliente_fd, item, cant, GENERAR)) {
            log_error(logger, "Error enviando tripulante");
        }
        sem_post(sem_padre);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        tipo_item r_item;
        uint16_t r_cant;
        if (!recv_item_cantidad(conexion_fd, &r_item, &r_cant)) {
            log_error(logger, "Error recibiendo tripulante");
        }

        CU_ASSERT_EQUAL(item, r_item);
        CU_ASSERT_EQUAL(cant, r_cant);
    }
}

void test_patota_new() {
    // A enviar
    uint32_t n_tripulantes = 3;
    size_t sz_s_tareas;
    void* s_tareas = serializar_contenido_archivo(
        &sz_s_tareas,
        "/media/sf_tp-2021-1c-...undefined/tests/src/tareas/tareasTest.txt",
        logger
    );
    if (s_tareas == NULL) {
        log_error(logger, "Error abriendo el archivo de tareas");
        CU_ASSERT_EQUAL(1,0);
        return;
    }
    t_list* posiciones = list_create();
    t_posicion* p1 = malloc(sizeof(t_posicion));
    t_posicion* p2 = malloc(sizeof(t_posicion));
    t_posicion* p3 = malloc(sizeof(t_posicion));
    p1->x = 14; p2->x = 27; p3->x = 49;
    p1->y = 41; p2->y = 72; p3->y = 94;
    list_add(posiciones, p1);
    list_add(posiciones, p2);
    list_add(posiciones, p3);

    // Inicializacion de semaforos compartidos (wtf? y bueno.)
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_patota(cliente_fd, n_tripulantes, s_tareas, sz_s_tareas, posiciones)) {
            log_error(logger, "Error enviando iniciar_patota");
        }
        sem_post(sem_padre);
        free(s_tareas);
        list_destroy_and_destroy_elements(posiciones, &free_t_posicion);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }

        // Para recibir
        uint32_t r_n_tripulantes;
        char* r_tareas;
        t_list* r_posiciones;
        if (!recv_patota(conexion_fd, &r_n_tripulantes, &r_tareas, &r_posiciones)) {
            log_error(logger, "Error recibiendo iniciar_patota");
        }

        // ASSERT QUE ANDUVO BIEN (y prints si es muy dificil assert-ar XD!)
        CU_ASSERT_EQUAL(n_tripulantes, r_n_tripulantes);
        CU_ASSERT_TRUE(strcmp((char*) s_tareas, r_tareas) == 0);
        printf("\n<<%s>>\nstrlen: %d\n", r_tareas, strlen(r_tareas));

        for (int i=0; i<list_size(posiciones); i++) {
            t_posicion *p_orig, *p_recv;
            p_orig = (t_posicion*) list_get(posiciones, i);
            p_recv = (t_posicion*) list_get(r_posiciones, i);
            CU_ASSERT_TRUE(t_posicion_equals(p_orig, p_recv));
        }

        // Frees
        free(r_tareas);
        list_destroy_and_destroy_elements(r_posiciones, &free_t_posicion);
    }
    list_destroy_and_destroy_elements(posiciones, &free_t_posicion);
    free(s_tareas);
}

void test_cambio_estado() {
    uint32_t id_tripulante=14, r_id_tripulante;
    t_status estado = EXEC, r_estado;

    // Inicializacion de semaforos compartidos (wtf? y bueno.)
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_cambio_estado(cliente_fd, id_tripulante, estado)) {
            log_error(logger, "Error enviando cambio de estado");
        }
        sem_post(sem_padre);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        if (!recv_cambio_estado(conexion_fd, &r_id_tripulante, &r_estado)) {
            log_error(logger, "Error recibiendo cambio de estado");
        }

        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
        CU_ASSERT_EQUAL(estado, r_estado);
        CU_ASSERT_EQUAL(cop, CAMBIO_ESTADO);
    }
}

void test_ida_y_vuelta_tareas() {
    uint32_t id_tripulante=8, r_id_tripulante;
    t_posicion* pos_tarea = malloc(sizeof(t_posicion));
    pos_tarea->x = 6; pos_tarea->y = 9;
    t_tarea *tarea = tarea_create("TEST", 14, pos_tarea, 99, "GENERAR_BASURA"), *r_tarea;

    free(pos_tarea);

    // Inicializacion de semaforos compartidos (wtf? y bueno.)
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_solicitar_tarea(cliente_fd, id_tripulante)) {
            log_error(logger, "Error enviando solicitar tarea");
        }
        sem_post(sem_padre);
        op_code cop;
        if (recv(cliente_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        CU_ASSERT_EQUAL(cop, TAREA);
        if (!recv_tarea(cliente_fd, &r_tarea)) {
            log_error(logger, "Error recibiendo tarea");
        }

        CU_ASSERT_TRUE(strcmp(r_tarea->nombre, tarea->nombre)==0);
        CU_ASSERT_TRUE(r_tarea->param == tarea->param);
        CU_ASSERT_TRUE(t_posicion_equals(r_tarea->pos, tarea->pos));
        CU_ASSERT_TRUE(r_tarea->duracion == tarea->duracion);
        CU_ASSERT_TRUE(r_tarea->tipo == tarea->tipo);

        puts("All asserts passed? Suspicious... Let's check manually:");
        print_t_tarea(r_tarea);
        puts("wtf!!");

        free_t_tarea(tarea);
        free_t_tarea(r_tarea);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        op_code cop;
        if (recv(conexion_fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_error(logger, "Error recibiendo cop %d", cop);
        }
        CU_ASSERT_EQUAL(cop, SOLICITAR_TAREA);

        if (!recv_uint32_t(conexion_fd, &r_id_tripulante)) {
            log_error(logger, "Error recibiendo tid");
        }
        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);

        if (!send_tarea(conexion_fd, tarea)) {
            log_error(logger, "Error enviando tarea");
        }
        free_t_tarea(tarea);
        exit(0);
    }
}

void test_patota_ack() {
    bool ack = true;

    // Inicializacion de semaforos compartidos (wtf? y bueno.)
    sem_t* sem_padre = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_padre == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }
    sem_t* sem_hijo = (sem_t*) mmap(
        0,
        sizeof(sem_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_SHARED,
        0,
        0
    );
    if ((void*)sem_hijo == MAP_FAILED) { perror("mmap"); exit(EX_OSERR); }

    sem_init(sem_padre, 1, 1);
    sem_init(sem_hijo, 1, 0);

    // F O R K E A M E
    pid_t pid = fork();

    // CLIENTE
    if (pid==0) {
        sem_wait(sem_hijo);
        if (!send_ack(cliente_fd, ack)) {
            log_error(logger, "Error enviando patota ACK");
        }
        sem_post(sem_padre);
        exit(0);
    }
    // SERVIDOR
    else {
        sem_wait(sem_padre);
        int conexion_fd = esperar_cliente(logger, "TEST", server_fd);
        sem_post(sem_hijo);
        sem_wait(sem_padre);
        if (conexion_fd == -1) {
            log_error(logger, "Error en la conexion");
        }
        bool r_ack;
        if (!recv_ack(conexion_fd, &r_ack)) {
            log_error(logger, "Error recibiendo patota ACK");
        }

        CU_ASSERT_EQUAL(ack, r_ack);
    }
}

/////////

CU_TestInfo tests_protocolo[] = {
    { "Test send/recv tripulante", test_tripulante },
    { "Test send/recv iniciar_self_en_patota", test_iniciar_self_en_patota },
    { "Test send/recv iniciar_patota NEW", test_patota_new },
    { "Test send/recv sabotaje", test_sabotaje },
    { "Test send/recv op_code", test_send_cop },
    { "Test send/recv tarea", test_tarea },
    { "Test send/recv movimiento", test_movimiento },
    { "Test send/recv bitacora", test_bitacora },
    { "Test send/recv inicio/fin+tarea", test_accion_tripulante_tarea },
    { "Test send/recv generar/consumir+item", test_generar_consumir_item },
    { "Test send/recv cambio estado", test_cambio_estado },
    { "Test ida y vuelta tareas", test_ida_y_vuelta_tareas },
    { "Test send/recv patota ACK", test_patota_ack },
    CU_TEST_INFO_NULL,
};