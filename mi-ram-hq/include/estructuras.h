#ifndef ESTRUCTURAS_MRH_H_
#define ESTRUCTURAS_MRH_H_

#include <stdint.h>

typedef struct {
	uint32_t pid;
	uint32_t dl_tareas;
} PCB_t;

typedef struct {
	uint32_t tid;
	char estado;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t id_sig_tarea;
	uint32_t dl_pcb;
} TCB_t;

#endif