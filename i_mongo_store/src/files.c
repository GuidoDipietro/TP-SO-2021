#include "../include/files.h"

void escribir_archivo(char* nombre, file_t* file) {
    char* path = concatenar_montaje_files(nombre);
    FILE* f = fopen(path, "wb");
    free(path);
    fwrite(&(file->size), sizeof(uint32_t), 1, f);
    fwrite(&(file->block_count), sizeof(uint32_t), 1, f);
    fwrite(&(file->caracter_llenado), sizeof(char), 1, f);
    fwrite(&(file->blocks), sizeof(uint32_t), file->block_count, f);
    fclose(f);
    log_info(logger, "%s - Metadata escrita", nombre);
}

void crear_archivo(char* nombre, char c) {
    file_t* file = malloc(sizeof(file_t));
    file->size = 0;
    file->block_count = 0;
    file->blocks = malloc(0);
    file->caracter_llenado = c;
    char* path = concatenar_montaje_files(nombre);
    escribir_archivo(nombre, file);
    free(path);
    free(file->blocks);
    free(file);
    log_info(logger, "Archivo creado - %s - Se llena con el caracter \'%c\'", nombre, c);
}

void print_file_t(file_t* file) {
    printf("\n");
    printf("SIZE: %d\nBLOCK_COUNT: %d\nCARACTER_LLENADO: %c\n",
        file->size,
        file->block_count,
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
    uint32_t* blocks = malloc(sizeof(uint32_t) * block_count);
    fread(&(file->caracter_llenado), sizeof(char), 1, f);
    fread(blocks, sizeof(uint32_t), block_count, f);

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
}

uint32_t espacio_libre_ultimo_bloque(file_t* file) {
    if(list_is_empty(file->blocks))
        return 0;

    return superbloque->block_size * file->block_count - file->size;
}

void write_to_file(open_file_t* file_data, void* content, uint32_t len) {
    pthread_mutex_lock(&(file_data->mutex_file));
    file_t* file = file_data->file;

    printf("\nA\n");

    uint32_t libre_ultimo_bloque;
    bool hay_ultimo_bloque = !list_is_empty(file->blocks);

    uint32_t* ultimo_bloque;
    if(hay_ultimo_bloque) {
        ultimo_bloque = list_get(file->blocks, 0);
        libre_ultimo_bloque = espacio_libre_ultimo_bloque(file);
    } else
        libre_ultimo_bloque = 0;
        

    printf("\nC\n");

    if(libre_ultimo_bloque >= len)
        append_to_block(content, *ultimo_bloque, superbloque->block_size - libre_ultimo_bloque, len);
    else {
        uint32_t restante = len - libre_ultimo_bloque;
        uint32_t bloques_a_pedir = ceil(restante / ((double) superbloque->block_size));
        uint32_t entra_ultimo_bloque_asignado = (1 - bloques_a_pedir) * superbloque->block_size + restante;

        if(hay_ultimo_bloque)
            append_to_block(content, *ultimo_bloque, superbloque->block_size - libre_ultimo_bloque, libre_ultimo_bloque);

        uint32_t* bloque_libre;
        for(uint16_t i = 0; i < bloques_a_pedir - 1; i++) {
            bloque_libre = malloc(sizeof(uint32_t));
            *bloque_libre = monitor_offset_bloque_libre();
            list_add(file->blocks, bloque_libre);
            escribir_bloque(
                content + superbloque->block_size * i,
                *bloque_libre,
                superbloque->block_size
            );
        }

        // Escribimos el ultimo bloque. Puede tener fragmentacion interna
        bloque_libre = malloc(sizeof(uint32_t));
        *bloque_libre = monitor_offset_bloque_libre();
        list_add(file->blocks, bloque_libre);
        escribir_bloque(
            content + superbloque->block_size * bloques_a_pedir - 1,
            *bloque_libre,
            entra_ultimo_bloque_asignado
        );
        file->block_count += bloques_a_pedir;
    }
    file->size += len;

    pthread_mutex_unlock(&(file_data->mutex_file));

}