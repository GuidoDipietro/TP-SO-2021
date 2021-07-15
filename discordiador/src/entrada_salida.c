#include "../include/entrada_salida.h"

sem_t SEM_IO_LIBRE;

// Esto es basicamente un planificador, pero de e/s
// Esta funcion, una vez iniciada, no deberia frenar nunca
void controlador_es() {
    log_info(main_log, "Controlador de E/S iniciado correctamente");
    sem_init(&SEM_IO_LIBRE, 0, 1);
    while (1) {
        sem_wait(&SEM_IO_LIBRE);
        sem_wait(&TRIPULANTE_EN_BLOQUEADOS);
        t_running_thread* thread = pop_cola_bloqueados();
        sem_post(&(thread->sem_pause));
    } 
    sem_destroy(&SEM_IO_LIBRE);
}

/*

1. Bloqueamos el tripulante. Esto es por precaucion ya que tan solo esperando al semaforo deberia funcionar correctamente.
2. Lo sacamos de la LISTA_HILOs
3. Avisamos al planificador que hay otro hilo libre
4. Actualizamos el estado del tripulante
5. Agregamos el t_running_thread a la LISTA_BLOQUEADOS para que lo maneje el controlador_es()

*/

void tarea_io(t_running_thread* thread, t_tripulante* t) {
    log_info(main_log, "El tripulante %d se ha bloqueado por I/O", t->tid);
    thread->blocked = true;
    remover_lista_hilos(t->tid);
    sem_post(&ACTIVE_THREADS);
    cambiar_estado(t, BLOCKED);
    push_cola_bloqueados(thread);
    sem_post(&TRIPULANTE_EN_BLOQUEADOS);

    sem_wait(&(thread->sem_pause));
    log_info(main_log, "El tripulante %d comenzo su I/O", t->tid);
    
    t_tarea* tarea = t->tarea;

    // Asco esto, pero no tengo tiempo para pensar algo mejor :)
    if(tarea->tipo == DESCARTAR_BASURA_T)
        send_descartar_basura(t->fd_i_mongo_store);
    else {
        tipo_item item;
        op_code opcode;
        if(tarea->tipo == GENERAR_BASURA_T || tarea->tipo == GENERAR_COMIDA_T || tarea->tipo == GENERAR_OXIGENO_T)
            opcode = GENERAR;
        else
            opcode = CONSUMIR;

        if(tarea->tipo == GENERAR_BASURA_T)
            item = BASURA;
        else if(tarea->tipo == GENERAR_COMIDA_T || tarea->tipo == CONSUMIR_COMIDA_T)
            item = COMIDA;
        else if(tarea->tipo == GENERAR_OXIGENO_T || tarea->tipo == CONSUMIR_OXIGENO_T)
            item = OXIGENO;
        send_generar_consumir(t->fd_i_mongo_store, item, tarea->param, opcode);
    }

    while((t->tarea)->duracion) {
        __asm__ volatile ("call ciclo");
        ((t->tarea)->duracion)--;
        log_info(main_log, "#%d I/O - %d remaining", t->tid, (t->tarea)->duracion);
    }

    sem_post(&SEM_IO_LIBRE); // Libero el dispositivo de I/O
    log_info(main_log, "El tripulante %d finalizo su I/O", t->tid);
}