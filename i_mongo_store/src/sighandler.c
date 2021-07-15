#include "../include/sighandler.h"

void set_signal_handlers() {
    signal(SIGINT, close_signal_handler);
    signal(SIGQUIT, close_signal_handler);
    signal(SIGSEGV, segfault_signal_handler);
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