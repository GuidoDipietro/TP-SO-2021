#ifndef INIT_FS_H
#define INIT_FS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/mman.h>
#include <commons/config.h>

#include "utils_fs.h"
#include "init_ims.h"

void formatear_fs();
void cargar_bloques();
void* crear_bitarray(uint32_t blocks);
void cargar_superbloque();

#endif