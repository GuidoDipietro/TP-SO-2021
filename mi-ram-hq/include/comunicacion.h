#ifndef COMUNICACION_MRH_H_
#define COMUNICACION_MRH_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../include/graphic.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"

int server_escuchar(t_log* logger, char* server_name, int server_socket);

#endif