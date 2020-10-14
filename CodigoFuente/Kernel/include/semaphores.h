#ifndef SEMAPHORES_H_
#include "lib.h"
#include "mem_manager.h"
#include "queue.h"

#define MAXNAME 50

extern int _xchg(int* lock, int value);

typedef struct sem_t {
    char name[MAXNAME];
    int value;
    int lock;
    Queue* chain;
} sem_t;
int sem_open(const char* name, int initial);
void sem_close(const char* name);
void printSemaphoreState();
void sem_wait(const char* name);
void sem_post(const char* name);

int find_sem_by_name(const char* name);
#endif