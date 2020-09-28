#ifndef SCHEDULER_H_
#include <stdint.h>

#include "process.h"

#define NULL ((void*)0)

typedef struct PCB {
    process* process;
    struct PCB* next;
    uint64_t cpuUsedTime;
} PCB;

uint64_t schedule(uint64_t rsp);
struct PCB* next();
void initProcess(void* entry_point, int argc, char* argv[]);
void removeProcess(uint64_t pid);
process* getCurrentProcess();
uint64_t getCurrentPID();
void blockProcess(uint64_t pid);

#endif