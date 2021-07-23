#include "../include/tareas.h"

void tarea_generar(tipo_item tipo, uint32_t cantidad) {
    sem_t sem;
    sem_init(&sem, 0, 0);
    agregar_controlador_disco(&sem);
    sem_wait(&sem);

    char c;
    char* nombre;
    if(tipo == OXIGENO) {
        c = 'O';
        nombre = "Oxigeno.ims";
    } else if(tipo == COMIDA) {
        c = 'C';
        nombre = "Comida.ims";
    } else if(tipo == BASURA) {
        c = 'B';
        nombre = "Basura.ims";
    }

    open_file_t* file_data = obtener_archivo(nombre); // Esta abierto ya?

    if(file_data == NULL) {
        // Intentamos abrirlo
        file_data = cargar_archivo(nombre);

        if(file_data == NULL) {
            // No existe, vamos a crearlo
            crear_archivo(nombre, c);
            file_data = cargar_archivo(nombre);
        }
    }
    generar_recurso(file_data, cantidad);
    cerrar_archivo(file_data);

    sem_destroy(&sem);
    sem_post(&DISCO_LIBRE);
}

void tarea_consumir(tipo_item tipo, uint32_t cantidad) {
    sem_t sem;
    sem_init(&sem, 0, 0);
    agregar_controlador_disco(&sem);
    sem_wait(&sem);

    char* nombre;
    if(tipo == OXIGENO)
        nombre = "Oxigeno.ims";
    else if(tipo == COMIDA)
        nombre = "Comida.ims";

    open_file_t* file_data = obtener_archivo(nombre);

    if(file_data == NULL) {
        file_data = cargar_archivo(nombre);

        if(file_data == NULL) {
            log_info(logger, "No existe %s. No se puede consumir", nombre);
            return;
        }
    }
    consumir_recurso(file_data, cantidad);
    cerrar_archivo(file_data);

    sem_destroy(&sem);
    sem_post(&DISCO_LIBRE);
}


void descartar_basura() {
    sem_t sem;
    sem_init(&sem, 0, 0);
    agregar_controlador_disco(&sem);
    sem_wait(&sem);

    open_file_t* file_data = obtener_archivo("Basura.ims");

    if(file_data == NULL) {
        file_data = cargar_archivo("Basura.ims");

        if(file_data == NULL) {
            log_info(logger, "No existe Basura.ims");
            return;
        }
    }
    eliminar_archivo(file_data);

    sem_destroy(&sem);
    sem_post(&DISCO_LIBRE);
}

void completar_bitacora(char* nombre, void* content, uint32_t len) {
    sem_t sem;
    sem_init(&sem, 0, 0);
    agregar_controlador_disco(&sem);
    sem_wait(&sem);

    bitacora_t* bit = cargar_bitacora(nombre);

    if(bit == NULL) {
        crear_bitacora(nombre);
        bit = cargar_bitacora(nombre);
    }
    write_to_bitacora(bit, content, len);
    escribir_bitacora(bit);
    cerrar_bitacora(bit);
    sem_destroy(&sem);
    sem_post(&DISCO_LIBRE);
    log_info(logger, "Escrita bitacora %s", nombre);
}

char* obtener_bitacora(char* nombre) {
    sem_t sem;
    sem_init(&sem, 0, 0);
    agregar_controlador_disco(&sem);
    sem_wait(&sem);

    bitacora_t* bit = cargar_bitacora(nombre);
    
    if(bit == NULL) {
        log_info(logger, "No existe la bitacora %s", nombre);
        sem_destroy(&sem);
        sem_post(&DISCO_LIBRE);
        return strdup("");
    }
    
    char* ret = recuperar_bitacora(bit);

    ret = realloc(ret, bit->size + 1);
    ret[bit->size] = '\0';

    sem_destroy(&sem);
    sem_post(&DISCO_LIBRE);
    log_info(logger, "Recuperada bitacora %s", nombre);

    return ret;
}