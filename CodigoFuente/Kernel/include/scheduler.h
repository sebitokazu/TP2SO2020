#ifndef SCHEDULER_H_
#include <stdint.h>

#include "process.h"

#define NULL ((void*)0)

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
int initPipedProcesses(void* entry_point1, char* argv1[], void* entry_point2, char* argv2[]);
int removeProcess(uint64_t pid);
process* getCurrentProcess();
uint64_t getCurrentPID();
uint64_t getForegroundPID();
void setForegroundPID(uint64_t pid);
int blockProcess(uint64_t pid);
int unblockProcess(uint64_t pid);
void my_exit();
void forceTT();
void freeAll();
void addToFreeList(PCB* pcb);
void yield();
void printProcesses();
int changePriority(qword pid, int priority);
void addToProcessList(process* process);

#endif