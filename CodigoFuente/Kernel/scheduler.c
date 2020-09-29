#include "scheduler.h"

#include "lib.h"
#include "mem_manager.h"
#include "time.h"
#include "video_driver.h"

static PCB* readyList = NULL;

unsigned int cant_process = 0;
uint64_t readyListTimeCount = 0, previousTimeCount = 0;

uint64_t schedule(uint64_t rsp) {
    if (readyList == NULL) return rsp;
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
    PCB* aux = readyList->next;
    while (aux->process->state != READY) {
        aux = aux->next;
    }
    return aux;
}

int blockProcess(uint64_t pid) {  //cambia el estado de ready a block y viceversa
    int i;
    PCB* aux = readyList;
    for (i = 0; i < cant_process && aux->process->pid != pid; i++)
        aux = aux->next;

    if (aux->process->pid == pid) {
        aux->process->state = (aux->process->state + 1) % 2;
        return 0;
    }
    //Deberia ejecutar el tt? Por si un proceso se bloquea a si mismo.

    return -1;
}

//solicita la creacion de un proceso, es a quien llama la syscall fork
int initProcess(void* entry_point, int argc, char* argv[]) {
    process* new_process = createProcess(entry_point, argc, argv);

    if (new_process == NULL)
        return -1;

    addToProcessList(new_process);
    return new_process->pid;
}

void addToProcessList(process* process) {
    PCB* pcb = (PCB*)my_malloc(sizeof(PCB*));

    if (pcb != NULL) {
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
}

int removeProcess(uint64_t pid) {
    PCB* previousPCB = readyList;
    PCB* readyListPCB = readyList->next;
    int i;

    for (i = 0; i < cant_process && readyListPCB->process->pid != pid; i++) {
        previousPCB = readyListPCB;
        readyListPCB = readyListPCB->next;
    }

    if (readyListPCB->process->pid == pid) {
        previousPCB->next = readyListPCB->next;
        freeProcess(readyListPCB->process);
        my_free(readyListPCB);
        cant_process--;
        return 0;
    }

    //Deberia ejecutar el tt? Por si un proceso se suicida

    return -1;
}

process* getreadyListProcess() {
    return readyList->process;
}

uint64_t getreadyListPID() {
    return readyList->process->pid;
}

void cpuUsedTime() {
    readyListTimeCount = seconds_elapsed();
    uint64_t elapsed = previousTimeCount - readyListTimeCount;
    previousTimeCount = readyListTimeCount;
    readyList->cpuUsedTime += elapsed;
}

void startScheduler() {
    ((int (*)(void))(getreadyListProcess()->rsp))();
}

void printProcesses() {
    if (readyList == NULL)
        return;
    PCB* current = readyList;
    int i;
    char pidBuff[20];
    for (i = 0; i < cant_process; i++) {
        drawWord(current->process->name);
        drawWord(" - PID:");
        intToStr(current->process->pid, pidBuff);
        drawWord(pidBuff);
        drawWord(" - RSP:");
        intToStr(current->process->rsp, pidBuff);
        drawWord(pidBuff);
        drawWord(" - Base:");
        intToStr(current->process->stack_base, pidBuff);
        drawWord(pidBuff);
        jumpLine();
        current = current->next;
    }
}