#include "../include/init_fs.h"

superbloque_t superbloque;
void* mem_map;
void* mem_cpy;

void cargar_superbloque() {
    char* path = concatenar_montaje("SuperBloque.ims");
    FILE* f = fopen(path, "r");
    fread(&superbloque.blocks, sizeof(uint32_t), 1, f);
    fread(&superbloque.block_size, sizeof(uint32_t), 1, f);
    uint32_t bytes = ceil(superbloque.blocks / 8.00); // el size del bitmap
    printf("\n%d - %d - bytes: %d\n", superbloque.blocks, superbloque.block_size, bytes);

    // Pasamos a cargar el bitmap a memoria
    raw_bitmap_t raw_bitmap = malloc(bytes);
    fread(raw_bitmap, bytes, 1, f);
    superbloque.bitarray = bitarray_create(raw_bitmap, bytes);

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
        MAP_SHARED,
        fd,
        0
    );
    fclose(f);

    //mem_cpy = malloc(len);
    //memcpy(mem_cpy, mem_map, len);
}

void crear_bloques() {
    char* path = concatenar_montaje("Blocks.ims");
    FILE* f = fopen(path, "wb+"); // Creamos el archivo

    free(path);
    uint64_t total_size = superbloque.blocks * superbloque.block_size; // En bytes
    uint8_t* p = malloc(1);
    *p = 0x0;
    fwrite(p, 1, total_size, f); // Llenamos el archivo de 0 -> memoria vacia
    free(p);
    fclose(f);
}