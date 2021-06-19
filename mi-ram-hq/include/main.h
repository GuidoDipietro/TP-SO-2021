#ifndef MI_RAM_HQ_MAIN_H
#define MI_RAM_HQ_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <curses.h>
#include <signal.h>

#include "graphic.h"
#include "init_mrhq.h"
#include "monitor_memoria.h"
#include "manejo_memoria.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"

void sighandler(int x) {
    switch (x) {
        case SIGUSR1:
            // dump_mp();
            eliminar_segmento_de_mp(0);
            break;
        case SIGUSR2:
            compactar_mp();
            break;
    }
}

#define SERVERNAME "MRH_SERVER"
#define NIVELNAME "AMONG-OS"

#endif
