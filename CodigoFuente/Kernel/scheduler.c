#include "scheduler.h"

#include "lib.h"
#include "mem_manager.h"
#include "time.h"
#include "video_driver.h"

extern void forceTimerTick();

static PCB* readyList = NULL;
static PCB* freeArray[50];
static int cantToFree = 0;

unsigned int cant_process = 0,
             current_cant_tt = 0;
//uint64_t readyListTimeCount = 0, previousTimeCount = 0;

uint64_t schedule(uint64_t rsp) {
    if (readyList == NULL) return rsp;

    if (current_cant_tt++ < readyList->priority)
        return rsp;

    current_cant_tt = 0;

    if (getCurrentPID() != 0 && ++(readyList->cpuUsedTime) % 90 == 0) {
        readyList->priority++;
        readyList->cpuUsedTime = 0;
    }

    readyList->process->rsp = rsp;

    if (readyList->process->state == RUNNING)
        readyList->process->state = READY;

    readyList = next();
    readyList->process->state = RUNNING;
    //se liberan procesos cuando hay un cambio de contexto
    freeAll();

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
        if (getCurrentPID() == pid)
            forceTT();
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
    pcb->priority = 0;

    if (pcb != NULL) {
        pcb->process = process;
        cant_process++;

        if (readyList == NULL) {
            readyList = pcb;
            readyList->next = readyList;
        } else {
            pcb->next = readyList->next;
            readyList->next = pcb;
            if (pcb->process->background == FOREGROUND && process->pid != 1) {
                readyList->process->state = BLOCKED;
                forceTT();
            }
        }
    }
}

void my_exit() {
    PCB* aux = readyList;
    int parent_pid = aux->process->parent_pid;

    if (parent_pid != NULL) {
        int i;
        aux = aux->next;
        for (i = 0; i < cant_process && aux->process->pid != parent_pid; i++)
            aux = aux->next;
        if (i != cant_process) {
            aux->process->state = READY;
        }
    }
    removeProcess(getCurrentPID());
    //agregar a una lista de procesos a liberar
    forceTT();
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
        readyListPCB->process->state = DEAD;
        addToFreeList(readyListPCB);
        //freeProcess(readyListPCB->process);
        //my_free(readyListPCB);

        cant_process--;
        return 0;
    }

    //Deberia ejecutar el tt? Por si un proceso se suicida

    return -1;
}

void addToFreeList(PCB* pcb) {
    freeArray[cantToFree++] = pcb;
}

void freeAll() {
    int i;
    for (i = 0; i < cantToFree; i++) {
        freeProcess(freeArray[i]->process);
        my_free(freeArray[i]);
    }
    cantToFree = 0;
}

process* getreadyListProcess() {
    return readyList->process;
}

uint64_t getCurrentPID() {
    return readyList->process->pid;
}

/*void cpuUsedTime() {
    readyListTimeCount = seconds_elapsed();
    uint64_t elapsed = previousTimeCount - readyListTimeCount;
    previousTimeCount = readyListTimeCount;
    readyList->cpuUsedTime += elapsed;
}*/

PCB* getPcbByPID(qword pid) {
    PCB* aux = readyList;
    int i;
    for (i = 0; i < cant_process && aux->process->pid != pid; i++)
        aux = aux->next;
    if (i == cant_process)
        return NULL;
    else
        return aux;
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
        drawWord(" - Priority:");
        intToStr(current->priority, pidBuff);
        drawWord(pidBuff);
        drawWord(" - State:");
        intToStr(current->process->state, pidBuff);
        drawWord(pidBuff);
        jumpLine();
        current = current->next;
    }
}

void forceTT() {
    current_cant_tt = readyList->priority;
    forceTimerTick();
}

int changePriority(qword pid, int priority) {
    if (priority < 0 || priority > 5)
        return -1;
    PCB* aux = getPcbByPID(pid);
    if (aux == NULL)
        return -1;
    else {
        aux->priority = priority;
        aux->cpuUsedTime = 0;
    }
}