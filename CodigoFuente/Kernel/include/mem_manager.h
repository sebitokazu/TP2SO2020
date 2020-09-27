#ifndef _MEM_MANAGER_H_
#include <stdio.h>

void* my_malloc(size_t size);
void my_free(void* ptr);
void initialize(void* base);
void printBitmap();
void checkMemoryStatus();
#endif