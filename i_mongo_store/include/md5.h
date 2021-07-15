#ifndef MD5_H
#define MD5_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char* md5sum(void* content, uint64_t len);

#endif