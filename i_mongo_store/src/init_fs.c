#include "../include/init_fs.h"

superbloque_t superbloque;
void* mem_map;
void* mem_cpy;

void cargar_superbloque() {
    char* path = concatenar_montaje("SuperBloque.ims");
    t_config* cfg = config_create(path);
    free(path);

    superbloque.blocks = config_get_int_value(cfg, "BLOCK_SIZE");
    superbloque.block_size = config_get_int_value(cfg, "BLOCKS");
    char* temp = config_get_string_value(cfg, "BITARRAY");

    if(temp == NULL || strcmp(temp, "") == 0) // Significa que no hay bitarray
        superbloque.bitarray = crear_bitarray(superbloque.blocks);
    else
        superbloque.bitarray = bitarray_create_with_mode(temp, strlen(temp), LSB_FIRST);
    //free(temp);
    config_destroy(cfg);
}

void* crear_bitarray(uint32_t blocks) {
    // Tiene que haber tantos bits como bloques
    // cada posicion de memoria es 1 byte (8 bits)
    // entonces hay que asignar blocks/8 bytes
    uint32_t len = ceil(blocks/8.00);
    void* str = malloc(len);
    memset(str, 0, len);
    return str;
}

void cargar_bloques() {
    char* path = concatenar_montaje("Blocks.ims");
    FILE* f = fopen(path, "rb+");

    if(f == NULL) { // No existe el archivo, iniciamos un FS formateado
        formatear_fs();
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

void formatear_fs() {
    superbloque.bitarray = crear_bitarray(superbloque.blocks);

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