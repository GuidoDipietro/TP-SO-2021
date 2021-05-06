#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "../shared/include/sockets.h"
#include "include/graphic.h"
#include "include/init_mrhq.h"

#define MODULENAME "MRH"
#define SERVERNAME "MRH_SERVER"
#define NIVELNAME "AMONG-OS"

static t_config_mrhq* initialize_cfg() {
    t_config_mrhq* cfg = malloc(sizeof(t_config_mrhq));
    cfg->ALGORITMO_REEMPLAZO = NULL;
    cfg->ESQUEMA_MEMORIA = NULL;
    cfg->PATH_SWAP = NULL;
    return cfg;
}

int main() {
    t_config_mrhq* cfg = initialize_cfg();
    t_log* logger = log_create("mi-ram-hq.log", MODULENAME, true, LOG_LEVEL_INFO);

    if(!cargar_configuracion(cfg, logger)) {
        cerrar_programa(cfg, logger);
        return EXIT_FAILURE;
    }

    // Aca encapsule codigo que habia antes. Por favor poner esto en funciones para que ande bien, gracias
    if(false){
        NIVEL* among_os_nivel;
        int tamanio_nivel[2];


        // **** INICIO SERVIDOR, RESERVO PUERTO Y ESPERO CONEXIONES ****
        int server_mrh = iniciar_servidor(
                logger,
                MODULENAME,
                "127.0.0.1",
                cfg->PUERTO
        );

        log_info(logger, "Servidor MRH inicializado");

        // **** PARTE GRAFICA ****
        among_os_nivel = iniciar_ventana(logger, NIVELNAME);
        nivel_gui_get_area_nivel(&tamanio_nivel[0], &tamanio_nivel[1]);


        // **** CONEXIONES ENTRANTES ****
        // **** PRUEBA DE DIBUJO EN VENTANA ****
        int conexion_entrante = esperar_cliente(logger, SERVERNAME, server_mrh);

        // PRUEBA PARA UNA CONEXION
        if(conexion_entrante != -1){
            personaje_crear(
                    among_os_nivel,
                    // ESTA INFO DEBERIA RECIBIRSE DESDE EL DISCORDIADOR (ID Y POSICION ACTUAL DE LOS TRIPULANTES)
                    'A',
                    posicion_random(tamanio_nivel[0]),
                    posicion_random(tamanio_nivel[1])
            );

        }

        nivel_gui_dibujar(among_os_nivel);

        // PRUEBA DE 'EXPULSAR CONEXION' (ESTO DEBERIA ACTIVARSE CON LA EXPULSION DE UN TRIPULANTE
        // ESTO SOLO LO DEJO DE PRUEBA
        int key;
        noecho();
        key = getch();
        if(key == 'W' || key == 'w') {
            item_borrar(among_os_nivel, 'A');
        }

        printw("Presiona cualquier tecla para salir...");

        nivel_gui_dibujar(among_os_nivel);

        getch();

        // **** LIBERO CONEXION ****
        liberar_conexion(&server_mrh);

        nivel_gui_terminar();
    }

    cerrar_programa(cfg, logger);

	return EXIT_SUCCESS;
}
