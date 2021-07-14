#include "../include/blocks.h"

pthread_mutex_t MUTEX_BLOCKS;

void escribir_bloque(void* content, uint32_t nro_bloque) {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    monitor_bitarray_set_bit(nro_bloque);
    memcpy(mem_cpy + superbloque->block_size * nro_bloque, content, superbloque->block_size);
    pthread_mutex_unlock(&MUTEX_BLOCKS);
}

static void sincronizar_bitarray() {
    pthread_mutex_lock(&MUTEX_BITARRAY);
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "wb+");
    fseek(f, 2 * sizeof(uint32_t), SEEK_SET);
    fwrite(superbloque->bitarray->bitarray, 1, superbloque->bytes_bitarray, f);
    fclose(f);
    log_info(logger, "Se sincronizo el SuperBloque.ims");
    free(path);
    pthread_mutex_unlock(&MUTEX_BITARRAY);
}

void sincronizar_fs() {
    pthread_mutex_lock(&MUTEX_BLOCKS);
    memcpy(mem_map, mem_cpy, superbloque->tamanio_fs);
    msync(mem_map, superbloque->tamanio_fs, MS_SYNC);
    log_info(logger, "Se sincronizo Blocks.ims");
    sincronizar_bitarray();
    pthread_mutex_unlock(&MUTEX_BLOCKS);
}

void sincronizador() {
    log_info(logger, "Sincronizador inicializado.");
    while(true) {
        sleep(cfg->TIEMPO_SINCRONIZACION);
        sincronizar_fs();
    }
}