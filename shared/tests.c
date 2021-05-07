// gcc tests.c src/protocolo.c -g -lcommons -o tests

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <commons/collections/list.h>

#include "include/protocolo.h"

void print_t_posicion(void* p) {
    t_posicion* t_p = (t_posicion*) p;
    printf("%d|%d\n", t_p->x, t_p->y);
}
void free_t_posicion(void* p) {
    free(p);
}

// void test_serializar_iniciar_patota() {
// 	uint8_t tripulantes = 6;
// 	char* filepath = "/home/etc/wow/such/a/long/path/name/holy/shites/ben/baron.txt";

// 	t_list* posiciones = list_create();
// 	t_posicion* p1 = malloc(sizeof(t_posicion));
//     t_posicion* p2 = malloc(sizeof(t_posicion));
//     t_posicion* p3 = malloc(sizeof(t_posicion));
//     t_posicion* p4 = malloc(sizeof(t_posicion));
//     t_posicion* p5 = malloc(sizeof(t_posicion));
//     t_posicion* p6 = malloc(sizeof(t_posicion));
//     p1->x=14; p2->x=27; p3->x=49; p4->x=73; p5->x=31; p6->x=99;
//     p1->y= 6; p2->y= 9; p3->y= 7; p4->y= 1; p5->y= 2; p6->y= 0;
//     list_add(posiciones, p1);
//     list_add(posiciones, p2);
//     list_add(posiciones, p3);
//     list_add(posiciones, p4);
//     list_add(posiciones, p5);
//     list_add(posiciones, p6);
//     //
//     puts("Antes:");
//     printf("%d\n%s\n", tripulantes, filepath);
//     list_iterate(posiciones, print_t_posicion);
//     puts("");
//     //
//     void* stream = serializar_iniciar_patota(tripulantes, filepath, posiciones);
//     //
//     puts("Durante:");
//     printf("%d\n%s\n", tripulantes, filepath);
//     list_iterate(posiciones, print_t_posicion);
//     puts("");
//     //
//     uint8_t r_tripulantes;
//     char* r_filepath;
//     t_list* r_posiciones;
//     deserializar_iniciar_patota(stream+sizeof(op_code), &r_tripulantes, &r_filepath, &r_posiciones);

//     puts("Despues:");
//     printf("%d\n%s\n", tripulantes, filepath);
//     list_iterate(posiciones, print_t_posicion);
//     puts("");
//     //
// 	list_destroy_and_destroy_elements(posiciones, *free_t_posicion);
// 	list_destroy_and_destroy_elements(r_posiciones, *free_t_posicion);
// 	free(stream);
// 	free(r_filepath);
// }

// void test_expulsar_tripulante() {
// 	uint8_t id = 14;
// 	uint8_t r_id;

// 	void* stream = serializar_expulsar_tripulante(id);
// 	deserializar_tripulante(stream+sizeof(op_code), &r_id);

// 	free(stream);

// 	printf("\n\n%d %d\n\n", id, r_id);
// }

void test_files() {
    char* path = "../discordiador/tareas/tareasEjemplo.txt";
    FILE* file = fopen(path,"r");
    char buf[100];

    while (fscanf(file, "%s", buf) != EOF) {
        printf("%s\n", buf);
    }

    fclose(file);
}

int main() {
	// test_serializar_iniciar_patota();
	// test_expulsar_tripulante();
    test_files();

	return EXIT_SUCCESS;
}