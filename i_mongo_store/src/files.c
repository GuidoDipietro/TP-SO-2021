#include "../include/files.h"

void free_file_t(file_t* file) {
    if(file->blocks != NULL)
        list_destroy_and_destroy_elements(file->blocks, free);
    free(file);
}

void free_open_file_t(open_file_t* file_data) {
    free(file_data->nombre);
    free_file_t(file_data->file);
    pthread_mutex_destroy(&(file_data->mutex_file));
    free(file_data);
}

static void interfaz_escribir_archivo(open_file_t* file_data) {
    escribir_archivo(file_data->nombre, file_data->file);
}

void guardar_metadata_todos() {
    monitor_iterar_lista_archivos((void*) interfaz_escribir_archivo);
}

void guardar_metadata_todos_ignorar_mutex() {
    list_iterate(OPEN_FILES, (void*) interfaz_escribir_archivo);
}

void escribir_archivo(char* nombre, file_t* file) {
    char* path = concatenar_montaje_files(nombre);
    FILE* f = fopen(path, "wb");
    free(path);
    fwrite(&(file->size), sizeof(uint32_t), 1, f);
    fwrite(&(file->block_count), sizeof(uint32_t), 1, f);
    fwrite(&(file->caracter_llenado), sizeof(char), 1, f);
    fwrite(file->md5, 32, 1, f);
    if(file->block_count > 0)
        for(uint32_t i = 0; i < file->block_count; i++) {
            uint32_t* d = list_get(file->blocks, i);
            fwrite(d, sizeof(uint32_t), 1, f);
        }
    fclose(f);
    log_info(logger, "%s - Metadata escrita", nombre);
    print_file_t(file);
}

void crear_archivo(char* nombre, char c) {
    file_t* file = malloc(sizeof(file_t));
    file->size = 0;
    file->block_count = 0;
    file->blocks = list_create();
    file->caracter_llenado = c;
    file->md5 = calloc(32, sizeof(char));
    //memset(file->md5, 0x00, 32);
    char* path = concatenar_montaje_files(nombre);
    escribir_archivo(nombre, file);
    free(path);
    list_destroy(file->blocks);
    free(file->md5);
    free(file);
    log_info(logger, "Archivo creado - %s - Se llena con el caracter \'%c\'", nombre, c);
}

void* recuperar_archivo(open_file_t* file_data) {
    file_t* file = file_data->file;

    if(file->size == 0)
        return NULL;

    //printf("\nA\n");
    void* content = malloc(file->size);

    //printf("\n~ 6 - %d - %d - %d ~\n", file->size, file->block_count, list_size(file->blocks));

    //printf("\nB\n");
    if(file->size > 0) {
        uint32_t* block_num;
        void* block_content;
        for(uint32_t i = 0; i < file->block_count - 1; i++) {
            block_num = list_get(file->blocks, i);
            block_content = leer_bloque(*block_num);
            memcpy(
                content + superbloque->block_size * i,
                block_content,
                superbloque->block_size
            );
        }
        //printf("\nC\n");
        
        //printf("\nD\n");
        block_num = list_get(file->blocks, file->block_count - 1);
        //printf("\nE\n");
        //printf("\n\nBLOCK NUM: %d\n\n", *block_num);
        block_content = leer_bloque(*block_num);
        //printf("\nF\n");
        //uint32_t content_size = superbloque->block_size - ((superbloque->block_size * file->block_count) - size);
        uint32_t content_size = (1 - file->block_count) * superbloque->block_size + file->size;
        memcpy(
            content + superbloque->block_size * (file->block_count - 1),
            block_content,
            content_size
        );
    }
    //printf("\nE\n");

    return content;
}

void print_file_t(file_t* file) {
    printf("\nSIZE: %d\nBLOCK_COUNT: %d | %d\nCARACTER_LLENADO: %c\n",
        file->size,
        file->block_count,
        list_is_empty(file->blocks) ? 0 : list_size(file->blocks),
        file->caracter_llenado
    );
}

void print_open_file_t(open_file_t* file_data) {
    printf("\n%s\n", file_data->nombre);
    printf("%d\n", file_data->refs);
    print_file_t(file_data->file);
}

open_file_t* cargar_archivo(char* nombre) {
    char* path = concatenar_montaje_files(nombre);
    FILE* f = fopen(path, "rb");
    free(path);

    if(f == NULL)
        return NULL;

    file_t* file = malloc(sizeof(file_t));

    fread(&(file->size), sizeof(uint32_t), 1, f);
    uint32_t block_count;
    fread(&block_count, sizeof(uint32_t), 1, f);
    file->block_count = block_count;

    fread(&(file->caracter_llenado), sizeof(char), 1, f);
    //fread(blocks, sizeof(uint32_t), block_count, f); 
    file->md5 = malloc(32);
    fread(file->md5, 32, 1, f);

    /*uint32_t* blocks[file->block_count];
    for(uint32_t i = 0; i < file->block_count; i++) {
        blocks[i] = malloc(sizeof(uint32_t));
        fread(blocks[i], sizeof(uint32_t), 1, f);
    }*/

    file->blocks = list_create();
    for(uint32_t i = 0; i < file->block_count; i++) {
        uint32_t* temp  = malloc(sizeof(uint32_t));
        fread(temp, sizeof(uint32_t), 1, f);
        list_add(file->blocks, temp);
    }

    //for(uint32_t i = 0; i < block_count; i++)
    //    list_add(file->blocks, (void*) blocks[i]);

    open_file_t* file_data = malloc(sizeof(open_file_t));
    file_data->file = file;
    file_data->refs = 1;
    file_data->nombre = strdup(nombre);
    pthread_mutex_init(&(file_data->mutex_file), NULL);
    print_open_file_t(file_data);
    agregar_archivo(file_data);
    fclose(f);
    return file_data;
}

uint32_t espacio_libre_ultimo_bloque(file_t* file) {
    if(list_is_empty(file->blocks))
        return 0;

    return superbloque->block_size * file->block_count - file->size;
}

void generar_recurso(open_file_t* file_data, uint32_t cantidad) {
    char* c = malloc(cantidad);
    //*c = (file_data->file)->caracter_llenado;
    memset(c, (file_data->file)->caracter_llenado, cantidad);
    write_to_file(file_data, c, cantidad);
    free(c);
    log_info(logger, "Se generaron %ld recursos en %s - Recursos disponibles: %d", cantidad, file_data->nombre, (file_data->file)->size);
}

void printear_bloques(uint16_t* t) { printf(" %d ", *t); }

void consumir_recurso(open_file_t* file_data, uint32_t cantidad) {
    file_t* file = file_data->file;

    //printf("\n\n^^ %d - %d ^^", file->size, cantidad);

    //("\n1 - %d - %d", file->size, file->block_count);

    if(cantidad >= file->size) { // Liberamos todo
        log_warning(
            logger,
            "%s - Recursos solicitados: %d - Recursos disponibles: %d - No quedan recursos disponibles",
            file_data->nombre,
            cantidad,
            file->size        
        );
        file->size = 0;
        file->block_count = 0;
        void liberar_bloque_lista(uint32_t* n) {
            liberar_bloque(*n);
            free(n);
        }
        list_clean_and_destroy_elements(file->blocks, (void*) liberar_bloque_lista);
        return;
    }

    uint32_t originales = cantidad;

    uint32_t size_ultimo_bloque = file->size - superbloque->block_size * (file->block_count - 1);
    uint32_t* nro_bloque = list_remove(file->blocks, file->block_count - 1);

    //printf("\n2 - %d - %d - %d", file->size, file->block_count, size_ultimo_bloque);

    if(cantidad >= size_ultimo_bloque) {
        liberar_bloque(*nro_bloque);
        file->block_count = file->block_count - 1;
        file->size -= size_ultimo_bloque;
    } else {
        //printf("\n\nACA\n\n");
        quitar_de_bloque(*nro_bloque, size_ultimo_bloque, cantidad);
        list_add(file->blocks, nro_bloque);
        file->size -= cantidad;
    }
    
    //file->size = cantidad >= file->size ? 0 : file->size - cantidad;
    cantidad = cantidad <= size_ultimo_bloque ? 0 : cantidad - size_ultimo_bloque;

    uint32_t a_eliminar = (uint32_t) (cantidad / ((double) superbloque->block_size));

    //printf("\n3 - %d - %d - %d - %d", file->size, file->block_count, cantidad, a_eliminar);

    if(cantidad > superbloque->block_size) {
        for(uint32_t i = 0; i < a_eliminar; i++) {
            nro_bloque = list_remove(file->blocks, file->block_count - 1);
            //printf("\n\nSacamos bloque");
            //list_iterate(file->blocks, printear_bloques);
            liberar_bloque(*nro_bloque);
            file->block_count = file->block_count - 1;
        }
        file->size -= a_eliminar * superbloque->block_size;
        cantidad -= superbloque->block_size * a_eliminar;
    }
    

    //printf("\n4 - %d - %d", file->size, file->block_count);

    if(cantidad > 0) {
        nro_bloque = list_get(file->blocks, file->block_count - 1);
        quitar_de_bloque(*nro_bloque, superbloque->block_size, cantidad);
        file->size -= cantidad;
    }

    //printf("\n5 - %d - %d", file->size, file->block_count);

    log_info(
        logger,
        "%s - Recursos consumidos: %d - Recursos restantes: %d",
        file_data->nombre,
        originales,
        file->size
    );

    //("\n\n");
    //list_iterate(file->blocks, printear_bloques);
    //printf("\n\n");
    file->block_count = list_size(file->blocks);

    void* content = recuperar_archivo(file_data);
    file->md5 = md5sum(content, file->size);
    //file->md5 = strdup("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    free(content);
}

void write_to_file(open_file_t* file_data, void* content, uint32_t len) {
    pthread_mutex_lock(&(file_data->mutex_file));
    file_t* file = file_data->file;

    uint32_t libre_ultimo_bloque;
    bool hay_ultimo_bloque = !list_is_empty(file->blocks);

    uint32_t* ultimo_bloque;
    if(hay_ultimo_bloque) {
        ultimo_bloque = list_get(file->blocks, file->block_count - 1);
        libre_ultimo_bloque = espacio_libre_ultimo_bloque(file);
    } else
        libre_ultimo_bloque = 0;
        

    if(libre_ultimo_bloque >= len)
        append_to_block(content, *ultimo_bloque, superbloque->block_size - libre_ultimo_bloque, len);
    else {
        uint32_t restante = len - libre_ultimo_bloque;
        uint32_t bloques_a_pedir = ceil(restante / ((double) superbloque->block_size));
        uint32_t entra_ultimo_bloque_asignado = (1 - bloques_a_pedir) * superbloque->block_size + restante;

        if(hay_ultimo_bloque && libre_ultimo_bloque > 0)
            append_to_block(content, *ultimo_bloque, superbloque->block_size - libre_ultimo_bloque, libre_ultimo_bloque);

        uint32_t* bloque_libre;
        for(uint16_t i = 0; i < bloques_a_pedir - 1; i++) {
            //printf("\nITERACION\n");
            bloque_libre = malloc(sizeof(uint32_t));
            *bloque_libre = monitor_offset_bloque_libre();
            list_add(file->blocks, bloque_libre);
            escribir_bloque(
                content + superbloque->block_size * i + libre_ultimo_bloque,
                *bloque_libre,
                superbloque->block_size
            );
        }

        // Escribimos el ultimo bloque. Puede tener fragmentacion interna
        bloque_libre = malloc(sizeof(uint32_t));
        *bloque_libre = monitor_offset_bloque_libre();
        list_add(file->blocks, bloque_libre);
        escribir_bloque(
            content + superbloque->block_size * (bloques_a_pedir - 1) + libre_ultimo_bloque,
            *bloque_libre,
            entra_ultimo_bloque_asignado
        );
        file->block_count += bloques_a_pedir;
    }
    file->size += len;

    pthread_mutex_unlock(&(file_data->mutex_file));
}

void cerrar_archivo(open_file_t* file_data) {
    escribir_archivo(file_data->nombre, file_data->file);
    (file_data->refs)--;

    if(!file_data->refs) { // Nadie mas lo referencia, lo podemos sacar
        remover_lista_archivos(file_data->nombre);
        //free_open_file_t(file_data);
    }
}

void eliminar_archivo(open_file_t* file_data) {
    file_t* file = file_data->file;
    char* name = strdup(file_data->nombre);
    void iterador_liberar_bloques(uint32_t* num) { liberar_bloque(*num); }
    list_iterate(file->blocks, (void*) iterador_liberar_bloques);
    remover_lista_archivos(file_data->nombre);
    char* path = concatenar_montaje_files(file_data->nombre);
    remove(path);
    free(path);
    free_open_file_t(file_data);
    log_info(logger, "Archivo %s eliminado.", name);
    free(name);
}