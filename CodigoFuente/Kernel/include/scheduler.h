#ifndef SCHEDULER_H_
#include <stdint.h>

#include "process.h"

//#define NULL ((void*)0)

typedef struct PCB {
    process* process;
    struct PCB* next;
    uint64_t cpuUsedTime;
    uint64_t cantCpuUsed;
    uint8_t priority;
} PCB;

uint64_t schedule(uint64_t rsp);
struct PCB* next();
int initProcess(void* entry_point, int argc, char* argv[]);
int removeProcess(uint64_t pid);
process* getCurrentProcess();
uint64_t getCurrentPID();
int blockProcess(uint64_t pid);
void my_exit();
void forceTT();
void freeAll();
void addToFreeList(PCB* pcb);

#endif