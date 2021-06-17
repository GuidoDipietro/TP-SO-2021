#include "../include/monitor_tablas.h"

extern pthread_mutex_t MUTEX_TS_PATOTAS;
extern pthread_mutex_t MUTEX_TS_TRIPULANTES;

extern t_list* ts_patotas;
extern t_list* ts_tripulantes;

/// TS PATOTAS

void list_add_tspatotas(ts_patota_t* elem) {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_add(ts_patotas, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

void asesinar_tspatotas() {
    pthread_mutex_lock(&MUTEX_TS_PATOTAS);
    list_destroy_and_destroy_elements(ts_patotas, &free_ts_patota_t);
    pthread_mutex_unlock(&MUTEX_TS_PATOTAS);
}

/// TS TRIPULANTES

void list_add_tstripulantes(ts_tripulante_t* elem) {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    list_add(ts_tripulantes, (void*) elem);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
}

void asesinar_tstripulantes() {
    pthread_mutex_lock(&MUTEX_TS_TRIPULANTES);
    list_destroy_and_destroy_elements(ts_tripulantes, &free_ts_tripulante_t);
    pthread_mutex_unlock(&MUTEX_TS_TRIPULANTES);
}