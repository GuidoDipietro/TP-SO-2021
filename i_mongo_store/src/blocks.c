#include "../include/blocks.h"

pthread_mutex_t MUTEX_BLOCKS;
sem_t sem_sabotaje_sincronizador;

void escribir_bloque(void* content, uint32_t nro_bloque, uint64_t size) {
    if(size > superbloque->block_size) {
        log_warning(logger, "Se quiso escribir %ld bytes al bloque %d", size, nro_bloque);
        return;
    }

    pthread_mutex_lock(&MUTEX_BLOCKS);
    monitor_bitarray_set_bit(nro_bloque);
    memset(mem_cpy + superbloque->block_size * nro_bloque, 0x00, superbloque->block_size); // Vaciamos el bloque por las dudas
    memcpy(mem_cpy + superbloque->block_size * nro_bloque, content, size);
    pthread_mutex_unlock(&MUTEX_BLOCKS);
    //log_info(logger, "Written block %ld", nro_bloque);
}

void append_to_block(void* content, uint32_t nro_bloque, uint64_t offset, uint64_t size) {
    if(size + offset > superbloque->block_size) {
        log_warning(logger, "Se quiso agregar %ld bytes al bloque %d en el offset %ld", size, nro_bloque, offset);
        return;
    }

    memcpy(mem_cpy + superbloque->block_size * nro_bloque + offset, content, size);
    pthread_mutex_unlock(&MUTEX_BLOCKS);
    //log_info(logger, "Appended %d bytes to block %d with offset %ld", size, nro_bloque, offset);
}

void* leer_bloque(uint32_t nro_bloque) {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    void* ret = malloc(superbloque->block_size);
    memcpy(ret, mem_cpy + superbloque->block_size * nro_bloque, superbloque->block_size);
    pthread_mutex_unlock(&MUTEX_BLOCKS);
    return ret;
}

uint32_t size_bloque(uint32_t nro_bloque, char c) {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    uint32_t inside_offset = 0;
    uint32_t block_pos = superbloque->block_size * nro_bloque;

    while(inside_offset < superbloque->block_size && *(mem_cpy + superbloque->block_size * nro_bloque + inside_offset) == c)
        inside_offset += sizeof(char); // Deberia ser 1, pero para mas portabilidad

    pthread_mutex_unlock(&MUTEX_BLOCKS);
    return inside_offset;
}

void quitar_de_bloque(uint32_t nro_bloque, uint32_t size_bloque,  uint32_t cantidad) {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    memset(
        mem_cpy + superbloque->block_size * nro_bloque + size_bloque - cantidad,
        0x00,
        cantidad
    );
    pthread_mutex_unlock(&MUTEX_BLOCKS);
}

void liberar_bloque(uint32_t nro_bloque) {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    memset(mem_cpy + superbloque->block_size * nro_bloque, 0x00, superbloque->block_size); // Vaciamos el bloque
    monitor_bitarray_clean_bit(nro_bloque);
    pthread_mutex_unlock(&MUTEX_BLOCKS);
    //log_info(logger, "Free block %ld", nro_bloque);
}

void sincronizar_datos_superbloque() {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "rb+");
    fwrite(&(superbloque->blocks), sizeof(uint32_t), 1, f);
    fwrite(&(superbloque->block_size), sizeof(uint32_t), 1, f);
    free(path);
    fclose(f);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
}

void sincronizar_bitarray() {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "rb+");
    fseek(f, 2 * sizeof(uint32_t), SEEK_SET);
    fwrite(superbloque->bitarray->bitarray, 1, superbloque->bytes_bitarray, f);
    fclose(f);
    free(path);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
}

void sincronizar_fs() {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    memcpy(mem_map, mem_cpy, superbloque->tamanio_fs);
    msync(mem_map, superbloque->tamanio_fs, MS_SYNC);
    log_info(logger, "Se sincronizo Blocks.ims");
    sincronizar_bitarray();
    log_info(logger, "Se sincronizo el SuperBloque.ims");
    pthread_mutex_unlock(&MUTEX_BLOCKS);
}

static void sincronizar_bitarray_ignorar_mutex() {
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "rb+");
    fseek(f, 2 * sizeof(uint32_t), SEEK_SET);
    fwrite(superbloque->bitarray->bitarray, 1, superbloque->bytes_bitarray, f);
    fclose(f);
    log_info(logger, "Se sincronizo el SuperBloque.ims");
    free(path);
}

void sincronizar_fs_ignorar_mutex() {
    memcpy(mem_map, mem_cpy, superbloque->tamanio_fs);
    msync(mem_map, superbloque->tamanio_fs, MS_SYNC);
    log_info(logger, "Se sincronizo Blocks.ims");
    sincronizar_bitarray_ignorar_mutex();
}

void sincronizador() {
    log_info(logger, "Sincronizador inicializado.");
    sem_init(&sem_sabotaje_sincronizador, 0, 0);
    while(true) {
        sleep(cfg->TIEMPO_SINCRONIZACION);
        if(saboteado)
            sem_wait(&sem_sabotaje_sincronizador);
        sincronizar_fs();
    }
}