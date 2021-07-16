#include "../include/sabotajes.h"

bool saboteado = false;
sem_t sem_sabotaje;

void verificar_integridad_archivo(char* nombre) {
    open_file_t* file_data = cargar_archivo(nombre);
    file_t* file = file_data->file;

    if(file_data == NULL) {
        log_info(logger, "No existe %s. No se verifica su integridad", nombre);
        return;
    }

    // SIZE

    uint32_t size_file = 0;
    void sumar_size(uint32_t* n) { size_file += size_bloque(*n, file->caracter_llenado); }
    file->size = size_file;

    // block_count y blocks

    file->block_count = list_size(file->blocks);

    // Blocks
    // TODO: Arreglar blocks

    cerrar_archivo(file_data);
}

void bloquear_fs() {
    saboteado = true;

    char* path;
    // CANTIDAD DE BLOQUES
    {
        path = concatenar_montaje("Blocks.ims");
        FILE* bp = fopen(path, "rb");
        free(path);
        fseek(bp, 0L, SEEK_END);
        superbloque->blocks = ftell(bp) / superbloque->block_size;
        log_info(logger, "Verificada la integridad de los bloques");
    }
    

    // BITMAP
    {
        void cargar_en_bitarray(uint32_t* bloque) { monitor_bitarray_set_bit(*bloque); }
        void abrir_archivo_y_cargar_en_bitarray(char* nombre) {
            open_file_t* file_data = cargar_archivo(nombre);

            if(file_data == NULL)
                return;

            list_iterate(
                ((file_data)->file)->blocks,
                (void*) cargar_en_bitarray
            );
            cerrar_archivo(file_data);
        }

        // Creamos un nuevo bitarray
        bitarray_destroy(superbloque->bitarray);
        uint32_t bytes = ceil(superbloque->blocks / 8.0);
        raw_bitmap_t b = malloc(bytes);
        memset(b, 0x00, bytes);
        superbloque->bitarray = bitarray_create(b, bytes);

        abrir_archivo_y_cargar_en_bitarray("Oxigeno.ims");
        abrir_archivo_y_cargar_en_bitarray("Basura.ims");
        abrir_archivo_y_cargar_en_bitarray("Comida.ims");

        // Para las bitacoras
        DIR *d;
        struct dirent* dir;
        path = path_bitacora();
        d = opendir(path);
        free(path);
        if(d) {
            while((dir = readdir(d)) != NULL) {
                if(strcmp(".", dir->d_name) == 0 || strcmp("..", dir->d_name) == 0 || strncmp("Tripulante", dir->d_name, 10) != 0)
                    continue;
                
                bitacora_t* bit = cargar_bitacora(dir->d_name);
                list_iterate(bit->blocks, (void*) cargar_en_bitarray);
                cerrar_bitacora(bit);
            }
            closedir(d);
        }
    }
    

    // Files

    {
        verificar_integridad_archivo("Oxigeno.ims");
        verificar_integridad_archivo("Basura.ims");
        verificar_integridad_archivo("Comida.ims");
    }

    saboteado = false;
    sem_post(&sem_sabotaje);
}