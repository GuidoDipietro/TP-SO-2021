#ifndef SABOTAJES_H
#define SABOTAJES_H

#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>

#include "files.h"
#include "bitacora.h"
#include "monitor_bitarray.h"

extern int fd_sabotajes;

void fsck();

#endif