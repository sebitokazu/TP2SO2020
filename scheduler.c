#include "scheduler.h"

#include "process.h"

static PCB* readyList = NULL;

unsigned int cant_process = 0;
uint64_t readyListTimeCount = 0, previousTimeCount = 0;

uint64_t schedule(uint64_t rsp) {
    cpuUsedTime();
    readyList->process->rsp = rsp;

    if (readyList->process->state == RUNNING)
        readyList->process->state = READY;

    readyList = next();
    readyList->process->state = RUNNING;

    return readyList->process->rsp;
}

//aca iria el algoritmo tipo Round Robin etc.
PCB* next() {
    return readyList->next;
}

//solicita la creacion de un proceso, es a quien llama la syscall fork
void initProcess(void* entry_point, int argc, char* argv[]) {
    process* new_process = createProcess(entry_point, argc, argv);
    addToProcessList(new_process);
}

void addToProcessList(process* process) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB*));
    pcb->process = process;
    cant_process++;

    if (readyList == NULL) {
        readyList = pcb;
        readyList->next = readyList;
    } else {
        pcb->next = readyList->next;
        readyList->next = pcb;
    }
}

void removeProcess(uint64_t pid) {
    PCB* previousPCB = readyList;
    PCB* readyListPCB = readyList->next;
    while (readyListPCB->process->pid != pid) {
        previousPCB = readyListPCB;
        readyListPCB = readyListPCB->next;
    }
    previousPCB->next = readyListPCB->next;
    freeProcess(readyListPCB->process);
    free(readyListPCB);
    cant_process--;
}

process* getreadyListProcess() {
    return readyList->process;
}

uint64_t getreadyListPID() {
    return readyList->process->pid;
}

void cpuUsedTime() {
    readyListTimeCount = timerTickSecs();
    uint64_t elapsed = previousTimeCount - readyListTimeCount;
    previousTimeCount = readyListTimeCount;
    readyList->cpuUsedTime += elapsed;
}