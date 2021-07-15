#include "../include/files.h"

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
    char* path = concatenar_montaje_files(nombre);
    escribir_archivo(nombre, file);
    free(path);
    list_destroy(file->blocks);
    free(file);
    log_info(logger, "Archivo creado - %s - Se llena con el caracter \'%c\'", nombre, c);
}

void* recuperar_archivo(open_file_t* file_data) {
    file_t* file = file_data->file;

    if(file->size == 0)
        return NULL;

    void* content = malloc(file->size);

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
    
    block_num = list_get(file->blocks, file->block_count - 1);
    block_content = leer_bloque(*block_num);
    //uint32_t content_size = superbloque->block_size - ((superbloque->block_size * file->block_count) - size);
    uint32_t content_size = (1 - file->block_count) * superbloque->block_size + file->size;
    memcpy(
        content + superbloque->block_size * (file->block_count - 1),
        block_content,
        content_size
    );
    return content;
}

void print_file_t(file_t* file) {
    printf("\n");
    printf("SIZE: %d\nBLOCK_COUNT: %d | %d\nCARACTER_LLENADO: %c\n",
        file->size,
        file->block_count,
        list_is_empty(file->blocks) ? 0 : list_size(file->blocks),
        file->caracter_llenado
    );
    printf("\n\n");
}

void print_open_file_t(open_file_t* file_data) {
    printf("\n%s\n", file_data->nombre);
    printf("%d\n", file_data->refs);
    print_file_t(file_data->file);
}

file_t* cargar_archivo(char* nombre) {
    file_t* file = malloc(sizeof(file_t));

    if(file == NULL)
        return NULL;

    char* path = concatenar_montaje_files(nombre);
    FILE* f = fopen(path, "rb");
    free(path);

    fread(&(file->size), sizeof(uint32_t), 1, f);
    uint32_t block_count;
    fread(&block_count, sizeof(uint32_t), 1, f);
    file->block_count = block_count;

    fread(&(file->caracter_llenado), sizeof(char), 1, f);
    //fread(blocks, sizeof(uint32_t), block_count, f); 

    uint32_t* blocks[file->block_count];
    for(uint32_t i = 0; i < file->block_count; i++) {
        blocks[i] = malloc(sizeof(uint32_t));
        fread(blocks[i], sizeof(uint32_t), 1, f);
    }

    file->blocks = list_create();

    for(uint32_t i = 0; i < block_count; i++)
        list_add(file->blocks, (void*) blocks[i]);

    open_file_t* file_data = malloc(sizeof(open_file_t));
    file_data->file = file;
    file_data->refs = 1;
    file_data->nombre = nombre;
    pthread_mutex_init(&(file_data->mutex_file), NULL);
    print_open_file_t(file_data);
    agregar_archivo(file_data);
    fclose(f);
}

uint32_t espacio_libre_ultimo_bloque(file_t* file) {
    if(list_is_empty(file->blocks))
        return 0;

    return superbloque->block_size * file->block_count - file->size;
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