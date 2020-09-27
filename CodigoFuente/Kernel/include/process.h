#ifndef PROCESS_H_

#include <stdint.h>

#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define WAITING 3
#define DEAD 4

typedef struct process {
    uint64_t rsp;
    uint64_t stack_base;
    uint64_t pid;
    uint8_t state;
    char *name;

} process;

process* createProcess(void* entry_point, int argc, char* argv[]);
void freeProcess(process* process);

#endif