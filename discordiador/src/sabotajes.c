#include "../include/sabotajes.h"

void iniciar_sabotaje(uint16_t signum) {
    printf("\n\nSabotaje iniciado\n\n");
}

void finalizar_sabotaje(uint16_t signum) {
    printf("\n\nSabotaje finalizado\n\n");
}

void set_signal_handlers() {
    signal(SIG_SABOTAJE, iniciar_sabotaje);
    signal(SIG_FIN_SABOTAJE,  finalizar_sabotaje);
}