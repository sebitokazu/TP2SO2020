#ifndef _MEM_MANAGER_H_
#include <stdio.h>

void* my_malloc(unsigned int size);
void my_free(void* ptr);
void initialize_memory();
void printBitmap();
void checkMemoryStatus();
#endif