#ifndef TP_2021_1C_UNDEFINED_CONFIG_H
#define TP_2021_1C_UNDEFINED_CONFIG_H

typedef struct {
    char* IP_MI_RAM_HQ;
    uint16_t PUERTO_MI_RAM_HQ;
    char* IP_I_MONGO_STORE;
    uint16_t PUERTO_I_MONGO_STORE;
    uint16_t GRADO_MULTITAREA;
    char* ALGORITMO;
    uint16_t QUANTUM;
    uint16_t DURACION_SABOTAJE;
    uint16_t RETARDO_CICLO_CPU;
} t_config_disc;

extern t_config_disc* DISCORDIADOR_CFG;

#endif
