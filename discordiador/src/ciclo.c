#include "../include/ciclo.h"

#include <stdio.h>

void ciclo_dis() {
    printf("\n%d\n", DISCORDIADOR_CFG->RETARDO_CICLO_CPU);
    sleep(DISCORDIADOR_CFG->RETARDO_CICLO_CPU);
}