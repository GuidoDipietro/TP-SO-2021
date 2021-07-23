#include "../include/entrada_salida.h"

sem_t SEM_IO_LIBRE;
//sem_t pausa_io;
// Esto es basicamente un planificador, pero de e/s
// Esta funcion, una vez iniciada, no deberia frenar nunca

t_queue* COLA;
pthread_mutex_t MUTEX;

void controlador_es() {
    log_info(main_log, "Controlador de E/S iniciado correctamente");
    sem_init(&SEM_IO_LIBRE, 0, 1);
    //sem_init(&pausa_io, 0, 0);
    pthread_mutex_init(&MUTEX, NULL);
    COLA = queue_create();
    while (1) {
        sem_wait(&SEM_IO_LIBRE);
        sem_wait(&TRIPULANTE_EN_BLOQUEADOS);
        pthread_mutex_lock(&MUTEX);
        sem_t* sem = queue_pop(COLA);
        pthread_mutex_unlock(&MUTEX);
        sem_post(sem);
        //sem_post(&(thread->sem_pause));
        //printf("\n\nIO LIBERADO\n\n");
    }
    sem_destroy(&SEM_IO_LIBRE);
    //sem_destroy(&pausa_io);
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
    //thread->blocked = true;
    remover_lista_hilos(t->tid);
    sem_post(&ACTIVE_THREADS);
    cambiar_estado(t, BLOCKED);

    sem_t block_sem;
    sem_init(&block_sem, 0, 0);
    pthread_mutex_lock(&MUTEX);
    queue_push(COLA, &block_sem);
    pthread_mutex_unlock(&MUTEX);

    push_cola_bloqueados(thread);
    sem_post(&TRIPULANTE_EN_BLOQUEADOS);

    log_info(main_log, "Tripulante %d espera su IO de %d", t->tid, (t->tarea)->duracion);
    
    sem_wait(&block_sem);
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
        if(SABOTAJE_ACTIVO)
            sem_wait(&pausar_io_en_sabotaje);

        //if(PLANIFICACION_BLOQUEADA)
        //    sem_wait(&pausa_io);

        __asm__ volatile ("call ciclo_dis");
        ((t->tarea)->duracion)--;
        log_info(main_log, "#%d I/O - %d remaining", t->tid, (t->tarea)->duracion);
    }

    log_info(main_log, "El tripulante %d finalizo su I/O", t->tid);

    //if(PLANIFICACION_BLOQUEADA)
    //    sem_wait(&pausa_io);

    pop_cola_bloqueados();
    sem_post(&SEM_IO_LIBRE); // Libero el dispositivo de I/O
    sem_destroy(&block_sem);
    //thread->blocked = false;
}