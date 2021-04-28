#ifndef CONSOLE_H_
#define CONSOLE_H_

typedef struct {
    void (*func)(char*);
    char* cmdName;
} commandMap;

// Inicia el menu
void menu_start();

#endif