#include "../include/sabotajes.h"

bool saboteado = false;
sem_t sem_sabotaje;
int fd_sabotajes = -1;
sem_t sem_inicio_fsck;

void verificar_blocks_archivo(open_file_t* file_data, file_t* file) {
    char* content = recuperar_archivo(file_data);
    char* md5 = md5sum(content, file->size);

    if(strcmp(md5, file->md5) == 0) {
        log_info(logger, "Hash MD5 de %s coincide.", file_data->nombre);
        free(content);
        free(md5);
        return;
    }

    // No coincide el MD5. Restauramos los bloques.

    for(uint32_t i = 0; i < file->block_count - 1; i++) {
        uint32_t* num_bloque = list_get(file->blocks, i);
        char* content = malloc(superbloque->block_size);
        memset(content, file->caracter_llenado, superbloque->block_size);
        escribir_bloque(content, *num_bloque, superbloque->block_size);
    }

    uint32_t restante = file->size - superbloque->block_size * (file->block_count - 1);

    if(restante > 0) {
        uint32_t* num_bloque = list_get(file->blocks, file->block_count - 1);
        char* content = malloc(restante);
        memset(content, file->caracter_llenado, restante);
        escribir_bloque(content, *num_bloque, restante);
    }
    log_info(logger, "%s restaurado", file_data->nombre);

    free(content);
    free(md5);
}

void verificar_integridad_archivo(char* nombre) {
    open_file_t* file_data = cargar_archivo(nombre);

    if(file_data == NULL) {
        log_info(logger, "No existe %s. No se verifica su integridad", nombre);
        return;
    }

    file_t* file = file_data->file;

    // SIZE

    //void print_bloques(uint32_t* b) { printf(" %d ", *b); }

    uint32_t size_file = 0;
    void sumar_size(uint32_t* n) { size_file += size_bloque(*n, file->caracter_llenado); }
    //list_iterate(file->blocks, print_bloques);
    list_iterate(file->blocks, (void*) sumar_size);
    file->size = size_file;
    log_info(logger, "Size de %s verificado. SIZE %d - BLOCK COUNT %d", file_data->nombre, file->size, file->block_count);

    // block_count y blocks

    file->block_count = list_size(file->blocks);

    // Blocks
    verificar_blocks_archivo(file_data, file);

    cerrar_archivo(file_data);
}

void fsck() {
    saboteado = true;

    sem_wait(&sem_inicio_fsck);

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
        log_info(logger, "Verificada la integridad del bitmap");
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