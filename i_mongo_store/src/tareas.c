#include "../include/tareas.h"

void tarea_generar(char* nombre, char c, uint32_t cantidad) {
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
}

void tarea_consumir(char* nombre, uint32_t cantidad) {
    open_file_t* file_data = obtener_archivo(nombre);

    if(file_data == NULL) {
        log_info(logger, "No existe %s. No se puede consumir", nombre);
        return;
    }
    consumir_recurso(file_data, cantidad);
    cerrar_archivo(file_data);
}


void descartar_basura() {
    open_file_t* file_data = obtener_archivo("Basura.ims");

    if(file_data == NULL) {
        file_data = cargar_archivo("Basura.ims");

        if(file_data == NULL) {
            log_info(logger, "No existe Basura.ims");
            return;
        }
    }
    eliminar_archivo(file_data);
}