#ifndef TOOLS_H
#define TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <commons/log.h>

int iniciar_server(void);
int escuchar_tripulantes();

#endif TOOLS_H_
