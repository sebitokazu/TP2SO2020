#ifndef MALLOC_H

#include "types.h"
#include <stdio.h>

void* malloc(size_t size);
void free(void* ptr);
#endif