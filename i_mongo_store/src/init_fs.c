#include "../include/init_fs.h"

superbloque_t superbloque;
void* mem_map;
void* mem_cpy;

static void crear_superbloque() {
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "wb+");
    free(path);

    uint32_t* p = malloc(sizeof(uint32_t));
    *p = cfg->BLOCKS;
    fwrite(p, sizeof(uint32_t), 1, f);
    *p = cfg->BLOCK_SIZE;
    fwrite(p, sizeof(uint32_t), 1, f);

    uint32_t bytes = ceil(cfg->BLOCKS / 8.0);
    raw_bitmap_t b = malloc(bytes);
    memset(b, 0x00, bytes);
    fwrite(b, 1, bytes, f);
    fclose(f);
}

void cargar_superbloque() {
    printf("\n-- %d - %d\n", cfg->BLOCKS, cfg->BLOCK_SIZE);
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "rb");

    bool nuevo_superbloque = false;

    if(f == NULL) {
        crear_superbloque();
        nuevo_superbloque = true;
        f = fopen(path, "rb");
    }

    fread(&superbloque.blocks, sizeof(uint32_t), 1, f);
    fread(&superbloque.block_size, sizeof(uint32_t), 1, f);
    superbloque.bytes_bitarray = ceil(superbloque.blocks / 8.00);

    if(nuevo_superbloque) crear_bloques();

    printf("\n%d - %d - bytes: %d\n", superbloque.blocks, superbloque.block_size, superbloque.bytes_bitarray);

    // Pasamos a cargar el bitmap a memoria
    raw_bitmap_t raw_bitmap = malloc(superbloque.bytes_bitarray);
    fread(raw_bitmap, superbloque.bytes_bitarray, 1, f);
    superbloque.bitarray = bitarray_create(raw_bitmap, superbloque.bytes_bitarray);

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
    uint64_t len = superbloque.blocks * superbloque.block_size;
    mem_map = mmap(
        NULL,
        len,
        PROT_READ | PROT_WRITE,
        0,
        fd,
        0
    );
    fclose(f);

    mem_cpy = malloc(len);
    memcpy(mem_cpy, mem_map, len);
}

void crear_bloques() {
    char* path = concatenar_montaje("Blocks.ims");
    FILE* f = fopen(path, "wb+"); // Creamos el archivo

    free(path);
    uint64_t total_size = superbloque.blocks * superbloque.block_size; // En bytes

    /*
    Dejo esto aca por las dudas
    
    uint8_t* p = malloc(1); // 1 byte
    *p = 0x0;

    for(uint64_t i = 0; i < total_size; i++)
        fwrite(p, 1, 1, f); // Llenamos el archivo de 0 -> memoria vacia

    free(p);
    */
    uint8_t* p = malloc(total_size);
    memset(p, 0x00, total_size);
    fwrite(p, total_size, 1, f);

    fclose(f);
}