#include "../include/test_protocolo.h"

#define IP "127.0.0.1"
#define PUERTO "6969"

static t_log* logger;
static int server_fd;
static int cliente_fd;

int crear_conexiones() {
    logger = log_create("tests.log", "TESTS", true, LOG_LEVEL_INFO);
    server_fd = iniciar_servidor(
            logger,
            "TEST_SERVER",
            IP,
            PUERTO
    );
    if (server_fd == -1) {
        printf("Fallo al crear server\n");
        return -1;
    }

    cliente_fd = crear_conexion(
            logger,
            "TEST_CLIENTE",
            IP,
            PUERTO
    );
    if (cliente_fd == 0) {
        printf("Fallo al crear cliente\n");
        close(server_fd);
    }

    puts("\nPROTOCOLO INIT DONE\n");

    return 0;
}

int limpiar_conexiones() {
    close(server_fd);
    close(cliente_fd);
    log_destroy(logger);
    return 0;
}

// Unos posta
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

void test_tripulante() {
    uint8_t id_tripulante=14, r_id_tripulante;

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
        if (!send_tripulante(cliente_fd, id_tripulante, EXPULSAR_TRIPULANTE)) {
            log_error(logger, "Error enviando tripulante");
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
        if (!recv_tripulante(conexion_fd, &r_id_tripulante)) {
            log_error(logger, "Error recibiendo tripulante");
        }

        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
    }
}

void test_tripulante2() {
    uint8_t id_tripulante=27, r_id_tripulante;

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
        if (!send_tripulante(cliente_fd, id_tripulante, EXPULSAR_TRIPULANTE)) {
            log_error(logger, "Error enviando tripulante");
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
        if (!recv_tripulante(conexion_fd, &r_id_tripulante)) {
            log_error(logger, "Error recibiendo tripulante");
        }

        CU_ASSERT_EQUAL(id_tripulante, r_id_tripulante);
    }
}

/////////

CU_TestInfo tests_protocolo[] = {
    // { "Test raw tareas to list (1)", test_raw_tareas_to_list_1 },
    // { "Test raw tareas to list (2)", test_raw_tareas_to_list_2 },
    { "Test send/recv tripulante", test_tripulante },
    { "Test send/recv tripulante (2)", test_tripulante2 },
    CU_TEST_INFO_NULL
};