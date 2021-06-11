#ifndef MI_RAM_HQ_MAIN_H
#define MI_RAM_HQ_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <curses.h>

#include "graphic.h"
#include "init_mrhq.h"
#include "monitor_memoria.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"

#define SERVERNAME "MRH_SERVER"
#define NIVELNAME "AMONG-OS"

extern t_config_mrhq* cfg;
extern t_log* logger;
extern t_list* segmentos_libres;

#endif