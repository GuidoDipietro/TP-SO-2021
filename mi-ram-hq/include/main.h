#ifndef MI_RAM_HQ_MAIN_H
#define MI_RAM_HQ_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <curses.h>

#include "graphic.h"
#include "init_mrhq.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"

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

t_config_mrhq* cfg;
t_log* logger;

#endif