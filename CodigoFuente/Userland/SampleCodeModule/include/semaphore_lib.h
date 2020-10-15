#ifndef _SEMAPHORE_LIB_H
#include "types.h"

int sem_open(const char* name, int size);

void sem_wait(const char* name);

void sem_post(const char* name);

void sem_close(const char* name);

void print_sem();
#endif