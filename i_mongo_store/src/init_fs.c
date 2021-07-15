#include "../include/init_fs.h"

uint8_t* mem_map;
uint8_t* mem_cpy;
t_superbloque* superbloque;

static void crear_superbloque() {
    log_info(logger, "Se ha generado un nuevo SUPERBLOQUE");
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "wb+");
    free((char*) path);

    uint32_t* p = malloc(sizeof(uint32_t));
    *p = cfg->BLOCKS;
    fwrite(p, sizeof(uint32_t), 1, f);
    *p = cfg->BLOCK_SIZE;
    fwrite(p, sizeof(uint32_t), 1, f);
    free(p);

    uint32_t bytes = ceil(cfg->BLOCKS / 8.0);
    raw_bitmap_t b = malloc(bytes);
    memset(b, 0x00, bytes);
    fwrite(b, 1, bytes, f);
    free(b);
    fclose(f);
}

void cargar_superbloque() {
    superbloque = malloc(sizeof(t_superbloque));
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "rb");

    bool nuevo_superbloque = false;

    if(f == NULL) {
        crear_superbloque();
        nuevo_superbloque = true;
        f = fopen(path, "rb");
    }
    free(path);

    uint32_t* temp = malloc(sizeof(uint32_t));

    fread(temp, sizeof(uint32_t), 1, f);
    superbloque->blocks = *temp;
    fread(temp, sizeof(uint32_t), 1, f);
    superbloque->block_size = *temp;
    free(temp);

    superbloque->bytes_bitarray = ceil(superbloque->blocks / 8.00);
    superbloque->tamanio_fs = superbloque->blocks * superbloque->block_size;

    if(nuevo_superbloque) crear_bloques();

    //printf("\n%d - %d - bytes: %d\n", superbloque->blocks, superbloque->block_size, superbloque->bytes_bitarray);

    // Pasamos a cargar el bitmap a memoria
    raw_bitmap_t raw_bitmap = malloc(superbloque->bytes_bitarray);
    fread(raw_bitmap, superbloque->bytes_bitarray, 1, f);
    superbloque->bitarray = bitarray_create(raw_bitmap, superbloque->bytes_bitarray);

    fclose(f);
}

void cargar_bloques() {
    char* path = concatenar_montaje("Blocks.ims");
    FILE* f = fopen(path, "rb+");

    if(f == NULL) { // No existe el archivo, iniciamos un FS formateado
        crear_bloques();
        f = fopen(path, "rb+");
    }
    free(path);

    int fd = fileno(f);

    //char* b = malloc(4);
    //fread(b, sizeof(char), 4, f);
    //printf("$%s$", b);
    //fseek(f, 0, SEEK_SET);

    mem_map = mmap(
        0,
        superbloque->tamanio_fs,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );
    fclose(f);

    mem_cpy = malloc(superbloque->tamanio_fs);
    memcpy(mem_cpy, mem_map, superbloque->tamanio_fs);
}

void crear_bloques() {
    char* path = concatenar_montaje("Blocks.ims");
    FILE* f = fopen(path, "wb+"); // Creamos el archivo

    free(path);
    /*
    Dejo esto aca por las dudas
    
    uint8_t* p = malloc(1); // 1 byte
    *p = 0x0;

    for(uint64_t i = 0; i < total_size; i++)
        fwrite(p, 1, 1, f); // Llenamos el archivo de 0 -> memoria vacia

    free(p);
    */
    uint8_t* p = malloc(superbloque->tamanio_fs);
    memset(p, 0x00, superbloque->tamanio_fs);
    fwrite(p, superbloque->tamanio_fs, 1, f);
    free(p);

    fclose(f);
    log_info(logger, "Se ha generado un nuevo Bloques.ims de %ld bytes", superbloque->tamanio_fs);
}