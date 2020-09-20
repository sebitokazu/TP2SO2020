#include "scheduler.h"

#include "process.h"

static PCB* current = NULL;

unsigned int cant_process = 0;

uint64_t schedule(uint64_t rsp) {
    current->process->rsp = rsp;

    if (current->process->state == RUNNING)
        current->process->state = READY;

    current = next();
    current->process->state = RUNNING;

    return current->process->rsp;
}

//aca iria el algoritmo tipo Round Robin etc.
PCB* next() {
    return current->next;
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

    if (current == NULL) {
        current = pcb;
        current->next = current;
    } else {
        pcb->next = current->next;
        current->next = pcb;
    }
}

void removeProcess(uint64_t pid) {
    PCB* previousPCB = current;
    PCB* currentPCB = current->next;
    while (currentPCB->process->pid != pid) {
        previousPCB = currentPCB;
        currentPCB = currentPCB->next;
    }
    previousPCB->next = currentPCB->next;
    freeProcess(currentPCB->process);
    free(currentPCB);
    cant_process--;
}

process* getCurrentProcess() {
    return current->process;
}

uint64_t getCurrentPID() {
    return current->process->pid;
}