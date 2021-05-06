#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <string.h>

typedef struct {
    void (*func)(char*, int*, int*);
    char* cmdName;
} commandMap;

// Inicia el menu
void menu_start(int*, int*);

#endif