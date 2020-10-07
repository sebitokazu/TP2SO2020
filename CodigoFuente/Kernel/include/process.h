#ifndef PROCESS_H_

#include <stdint.h>

#define READY 0
#define BLOCKED 1
#define RUNNING 2
//#define WAITING 3
#define DEAD 4

#define FOREGROUND 0
#define BACKGROUND 1

#define MAX_NAME 50

typedef struct process {
    uint64_t rsp;
    uint64_t stack_base;
    uint64_t pid;
    uint64_t parent_pid;
    uint8_t state;
    uint8_t background;
    char name[MAX_NAME];
} process;

process* createProcess(void* entry_point, int argc, char* argv[]);
void freeProcess(process* process);
void yield();

#endif