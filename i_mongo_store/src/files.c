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