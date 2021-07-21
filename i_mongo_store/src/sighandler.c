#include "../include/sighandler.h"

void set_signal_handlers() {
    signal(SIGINT, close_signal_handler);
    signal(SIGQUIT, close_signal_handler);
    //signal(SIGSEGV, segfault_signal_handler);
    signal(SIGUSR1, sabotaje_signal_handler);
}

t_posicion* pos_sabotaje() {
    /*if(list_is_empty(cfg->POSICIONES_SABOTAJE))
        return NULL;

    return list_remove(cfg->POSICIONES_SABOTAJE, 0);*/
    static uint16_t i = 0;

    if(i == list_size(cfg->POSICIONES_SABOTAJE))
        i = 0;

    t_posicion* ret = list_get(cfg->POSICIONES_SABOTAJE, i);
    
    i++;
    return ret;
}

void sabotaje_signal_handler() {
    t_posicion* pos = pos_sabotaje();

    if(pos == NULL) {
        log_info(logger, "No quedan posiciones de sabotajes");
        return;
    }
    log_info(logger, "Sabotaje iniciado");
    pthread_t HILO_FSCK;
    pthread_create(&HILO_FSCK, NULL, (void*) fsck, NULL);
    pthread_detach(HILO_FSCK);
    send_sabotaje(fd_sabotajes, pos);
    //free_t_posicion(pos);
}

void close_signal_handler() {
    pthread_cancel(HILO_SINCRONIZADOR);
    sincronizar_fs();
    guardar_metadata_todos();
    cerrar_programa();
    exit(0);
}

void segfault_signal_handler() {
    pthread_cancel(HILO_SINCRONIZADOR);
    sincronizar_fs_ignorar_mutex();
    guardar_metadata_todos_ignorar_mutex();
    cerrar_programa();
}