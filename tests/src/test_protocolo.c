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

// Tests para probar una util
void test_raw_tareas_to_list_1() {
    char* texto =
        "GENERAR_OXIGENO 12;2;3;5\r\n"
        "CONSUMIR_OXIGENO 120;23;1\r\n"
        "GENERAR_COMIDA 4;2;3;1\r\n"
        "GENERAR_BASURA 12;2;3;\r\n"
        "DESCARTAR_BASURA;3;1;7\r\n"
        "TAREA_INVENTADA 14;27;49;3\r\n"
        "OTRA_TRUCHA;6;6;6\0"
    ;

    t_list* tareas = raw_tareas_to_list(texto);

    puts("\nTexto original:");
    for (int i=0; texto[i]!='\0'; putchar(texto[i++]));
    puts("\nContenido del t_list*:");
    list_iterate(tareas, (void*) print_t_tarea);

    list_destroy_and_destroy_elements(tareas, (void*) free_t_tarea);
}

void test_raw_tareas_to_list_2() {
    char* texto =
        "GENERAR_BASURA 12;2;3;\r\n"
        "DESCARTAR_BASURA;3;1;7\r\n"
        "TAREA_QUE_DEBERIA_ROMPER_COSAS\r\n"
        "TAREA_INVENTADA 14;27;49;3\r\n"
        "OTRA_ROMPEDORA 3;;\r\n"
        "UNA_TAREA_RARISIMA;;;;;;;\r\n"
        "OTRA_TRUCHA;6;6;6\0"
    ;

    t_list* tareas = raw_tareas_to_list(texto);

    puts("\n\nTexto original:");
    for (int i=0; texto[i]!='\0'; putchar(texto[i++]));
    puts("\n\nContenido del t_list*:");
    list_iterate(tareas, (void*) print_t_tarea);

    list_destroy_and_destroy_elements(tareas, (void*) free_t_tarea);
}

// TESTS DE SEND-RECV CON DOS PROCESOS
void test_tripulante() {
    uint8_t id_tripulante=14, r_id_tripulante;

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
    uint8_t id_tripulante=69, r_id_tripulante;
    uint8_t id_patota=13, r_id_patota;

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

// y muuuchos, muuuchos mas!

/////////

CU_TestInfo tests_protocolo[] = {
    // { "Test raw tareas to list (1)", test_raw_tareas_to_list_1 },
    // { "Test raw tareas to list (2)", test_raw_tareas_to_list_2 },
    { "Test send/recv tripulante", test_tripulante },
    { "Test send/recv iniciar_self_en_patota", test_iniciar_self_en_patota },
    CU_TEST_INFO_NULL
};