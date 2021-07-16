#ifndef SIGHANDLER_H
#define SIGHANDLER_H

#include <signal.h>
#include <pthread.h>

#include "files.h"
#include "blocks.h"
#include "sabotajes.h"

void close_signal_handler();
void segfault_signal_handler();
void set_signal_handlers();
void sabotaje_signal_handler();

#endif