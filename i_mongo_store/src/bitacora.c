#include "../include/bitacora.h"

void escribir_bitacora(bitacora_t* bitacora) {
    char* path = concatenar_montaje_bitacora(bitacora->nombre);
    FILE* f = fopen(path, "wb");
    free(path);
    fwrite(&(bitacora->size), sizeof(uint32_t), 1, f);
    if(bitacora->block_count > 0)
        for(uint32_t i = 0; i < bitacora->block_count; i++) {
            uint32_t* ptr = list_get(bitacora->blocks, i);
            fwrite(ptr, sizeof(uint32_t), 1, f);
        }
            
    fclose(f);
    log_info(logger, "Escrita la metadata de bitacora - %s", bitacora->nombre);
}

void crear_bitacora(char* nombre) {
    char* path = concatenar_montaje_bitacora(nombre);
    FILE* f = fopen(path, "wb");
    free(path);
    fclose(f);
    bitacora_t* bitacora = malloc(sizeof(bitacora_t));
    bitacora->nombre = strdup(nombre);
    bitacora->size = 0;
    bitacora->block_count = 0;
    bitacora->blocks = list_create();
    escribir_bitacora(bitacora);
    list_destroy(bitacora->blocks);
    free(bitacora->nombre);
    free(bitacora);
}

bitacora_t* cargar_bitacora(char* nombre) {
    char* path = concatenar_montaje_bitacora(nombre);
    FILE* f = fopen(path, "rb");
    free(path);

    if(f == NULL)
        return NULL;

    bitacora_t* ret = malloc(sizeof(bitacora_t));
    fread(&(ret->size), sizeof(uint32_t), 1, f);
    ret->blocks = list_create();
    ret->nombre = strdup(nombre);
    ret->block_count = ceil(ret->size / ((double) superbloque->block_size));
    for(uint32_t i = 0; i < ret->block_count; i++) {
        uint32_t* temp = malloc(sizeof(uint32_t));
        fread(temp, sizeof(uint32_t), 1, f);
        list_add(ret->blocks, temp);
    }

    fclose(f);
    return ret;
}

// Si, es la misma funcion. Pero necesito que esto funcione, no que sea lindo
void write_to_bitacora(bitacora_t* file, void* content, uint32_t len) {
    uint32_t libre_ultimo_bloque;
    bool hay_ultimo_bloque = !list_is_empty(file->blocks);

    uint32_t* ultimo_bloque;
    if(hay_ultimo_bloque) {
        ultimo_bloque = list_get(file->blocks, file->block_count - 1);
        libre_ultimo_bloque = superbloque->block_size * file->block_count - file->size;
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
}

void cerrar_bitacora(bitacora_t* bitacora) {
    escribir_bitacora(bitacora);
    list_destroy_and_destroy_elements(bitacora->blocks, free);
    free(bitacora->nombre);
    free(bitacora);
}

void* recuperar_bitacora(bitacora_t* file) {
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
        free(block_content);
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
    free(block_content);
    log_info(logger, "Se recupero el contenido de %s", file->nombre);
    return content;
}
